/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2004, The KNotes Developers

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*******************************************************************/

#include <qclipboard.h>
#include <qptrlist.h>
#include <qtooltip.h>

#include <kdebug.h>
#include <kaction.h>
#include <kxmlguifactory.h>
#include <ksystemtray.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kpopupmenu.h>
#include <khelpmenu.h>
#include <kkeydialog.h>
#include <kglobalaccel.h>
#include <ksimpleconfig.h>
#include <kwin.h>
#include <kextsock.h>

#include <libkcal/journal.h>

#include "knotesapp.h"
#include "knote.h"
#include "knoteconfig.h"
#include "knoteconfigdlg.h"
#include "knotesglobalconfig.h"
#include "knoteslegacy.h"
#include "knotesnetrecv.h"

#include "knotes/resourcemanager.h"


int KNotesApp::KNoteActionList::compareItems( QPtrCollection::Item s1, QPtrCollection::Item s2 )
{
    if ( ((KAction*)s1)->text() == ((KAction*)s2)->text() )
        return 0;
    return ( ((KAction*)s1)->text() < ((KAction*)s2)->text() ? -1 : 1 );
}


KNotesApp::KNotesApp()
    : DCOPObject("KNotesIface"), QLabel( 0, 0, WType_TopLevel ),
      KXMLGUIBuilder( this ),
      m_defaultConfig( 0 ), m_listener( 0 )
{
    connect( kapp, SIGNAL(lastWindowClosed()), kapp, SLOT(quit()) );

    m_noteList.setAutoDelete( true );
    m_noteActions.setAutoDelete( true );

    // create the dock widget...
    KWin::setSystemTrayWindowFor( winId(), qt_xrootwin() );
    QToolTip::add( this, i18n( "KNotes: Sticky notes for KDE." ) );
    setBackgroundMode( X11ParentRelative );
    setPixmap( KSystemTray::loadIcon( "knotes" ) );

    // create the GUI...
    new KAction( i18n("New Note"), "filenew", 0,
        this, SLOT(newNote()), actionCollection(), "new_note" );
    new KAction( i18n("New Note From Clipboard"), "editpaste", 0,
        this, SLOT(newNoteFromClipboard()), actionCollection(), "new_note_clipboard" );
    new KHelpMenu( this, kapp->aboutData(), false, actionCollection() );

    KStdAction::preferences( this, SLOT(slotPreferences()), actionCollection() );
    KStdAction::keyBindings( this, SLOT(slotConfigureAccels()), actionCollection() );
    KStdAction::quit( this, SLOT(slotQuit()), actionCollection() )->setShortcut( 0 );

    setXMLFile( QString( instance()->instanceName() + "ui.rc" ) );

    m_guiFactory = new KXMLGUIFactory( this, this, "guifactory" );
    m_guiFactory->addClient( this );

    m_context_menu = static_cast<KPopupMenu*>(m_guiFactory->container( "knotes_context", this ));
    m_note_menu = static_cast<KPopupMenu*>(m_guiFactory->container( "notes_menu", this ));

    // create accels for global shortcuts
    m_globalAccel = new KGlobalAccel( this, "global accel" );
    m_globalAccel->insert( "global_new_note", i18n("New Note"), "",
                           ALT+SHIFT+Key_N, ALT+SHIFT+Key_N ,
                           this, SLOT(newNote()), true, true );
    m_globalAccel->insert( "global_new_note_clipboard", i18n("New Note From Clipboard"), "",
                           ALT+SHIFT+Key_C, ALT+SHIFT+Key_C,
                           this, SLOT(newNoteFromClipboard()), true, true );

    m_globalAccel->readSettings();

    KConfig *config = KGlobal::config();
    config->setGroup( "Global Keybindings" );
    m_globalAccel->setEnabled( config->readBoolEntry( "Enabled", true ) );

    updateGlobalAccels();

    // clean up old config files
    KNotesLegacy::cleanUp();

    // create the resource manager
    m_manager = new KNotesResourceManager();
    connect( m_manager, SIGNAL(sigRegisteredNote( KCal::Journal * )),
             this,      SLOT(createNote( KCal::Journal * )) );

    // read the notes
    m_manager->load();

    kapp->installEventFilter( this );

    // create the socket and possibly start listening for connections
    m_listener = new KExtendedSocket();
    m_listener->setSocketFlags( KExtendedSocket::passiveSocket | KExtendedSocket::inetSocket );
    connect( m_listener, SIGNAL(readyAccept()), SLOT(acceptConnection()) );
    updateNetworkListener();

    if ( m_noteList.count() == 0 && !kapp->isRestored() )
        newNote();
}

KNotesApp::~KNotesApp()
{
    saveNotes();

    blockSignals( true );
    m_noteList.clear();
    blockSignals( false );

    delete m_listener;
    delete m_defaultConfig;
    delete m_manager;
}

bool KNotesApp::commitData( QSessionManager& )
{
    saveConfigs();
    return true;
}

// -------------------- public DCOP interface -------------------- //

QString KNotesApp::newNote( const QString& name, const QString& text )
{
    // create the new note
    KCal::Journal *note = new KCal::Journal();

    // new notes have the current date/time as title if none was given
    if ( !name.isEmpty() )
        note->setSummary( name );
    else
        note->setSummary( KGlobal::locale()->formatDateTime( QDateTime::currentDateTime() ) );

    // the body of the note
    note->setDescription( text );

    m_manager->addNewNote( note );

    showNote( note->uid() );

    return note->uid();
}

QString KNotesApp::newNoteFromClipboard( const QString& name )
{
    const QString& text = KApplication::clipboard()->text();
    return newNote( name, text );
}

void KNotesApp::showNote( const QString& id ) const
{
    KNote* note = m_noteList[id];
    if ( note )
        showNote( note );
    else
        kdWarning(5500) << "No note with id: " << id << endl;
}

void KNotesApp::hideNote( const QString& id ) const
{
    KNote* note = m_noteList[id];
    if ( note )
        note->hide();
    else
        kdWarning(5500) << "No note with id: " << id << endl;
}

void KNotesApp::killNote( const QString& id, bool force )
{
    KNote* note = m_noteList[id];
    if ( note )
        note->slotKill( force );
    else
        kdWarning(5500) << "No note with id: " << id << endl;
}

// "bool force = false" doesn't work with dcop
void KNotesApp::killNote( const QString& id )
{
    killNote( id, false );
}

QMap<QString,QString> KNotesApp::notes() const
{
    QMap<QString,QString> notes;
    QDictIterator<KNote> it( m_noteList );

    for ( ; it.current(); ++it )
        notes.insert( it.current()->noteId(), it.current()->name() );

    return notes;
}

QString KNotesApp::name( const QString& id ) const
{
    KNote* note = m_noteList[id];
    if ( note )
        return note->name();
    else
        return QString::null;
}

QString KNotesApp::text( const QString& id ) const
{
    KNote* note = m_noteList[id];
    if ( note )
        return note->text();
    else
        return QString::null;
}

void KNotesApp::setName( const QString& id, const QString& newName )
{
    KNote* note = m_noteList[id];
    if ( note )
        note->setName( newName );
    else
        kdWarning(5500) << "No note with id: " << id << endl;
}

void KNotesApp::setText( const QString& id, const QString& newText )
{
    KNote* note = m_noteList[id];
    if ( note )
        note->setText( newText );
    else
        kdWarning(5500) << "No note with id: " << id << endl;
}

void KNotesApp::sync( const QString& app )
{
    QDictIterator<KNote> it( m_noteList );

    for ( ; it.current(); ++it )
        it.current()->sync( app );
}

bool KNotesApp::isNew( const QString& app, const QString& id ) const
{
    KNote* note = m_noteList[id];
    if ( note )
        return note->isNew( app );
    else
        return false;
}

bool KNotesApp::isModified( const QString& app, const QString& id ) const
{
    KNote* note = m_noteList[id];
    if ( note )
        return note->isModified( app );
    else
        return false;
}


// ------------------- protected methods ------------------- //

void KNotesApp::mousePressEvent( QMouseEvent* e )
{
    if ( !rect().contains( e->pos() ) )
        return;

    switch ( e->button() )
    {
    case LeftButton:
        if ( m_noteList.count() == 1 )
        {
            QDictIterator<KNote> it( m_noteList );
            showNote( it.toFirst() );
        }
        else if ( m_note_menu->count() > 0 )
            m_note_menu->popup( e->globalPos() );
        break;
    case MidButton:
        newNote();
        break;
    case RightButton:
        m_context_menu->popup( e->globalPos() );
    default: break;
    }
}

bool KNotesApp::eventFilter( QObject* o, QEvent* ev )
{
    if ( ev->type() == QEvent::KeyPress )
    {
        QKeyEvent* ke = (QKeyEvent*)ev;

        if ( ke->key() == Key_BackTab )         // Shift+Tab
        {
            // show next note
            QDictIterator<KNote> it( m_noteList );
            KNote *first = it.toFirst();
            for ( ; it.current(); ++it )
                if ( it.current()->hasFocus() )
                {
                    if ( ++it )
                        showNote( it.current() );
                    else
                        showNote( first );
                    break;
                }

            ke->accept();
            return true;
        }
        else
            ke->ignore();
    }

    return QLabel::eventFilter( o, ev );
}


// -------------------- protected slots -------------------- //

void KNotesApp::slotShowNote()
{
    // tell the WM to give this note focus
    showNote( QString::fromUtf8( sender()->name() ) );
}

void KNotesApp::slotPreferences()
{
    // reuse the dialog if possible
    if ( KNoteConfigDlg::showDialog( "KNotes Default Settings" ) )
        return;

    // create the KNoteConfig if needed
    if ( !m_defaultConfig )
    {
        QString configFile = KGlobal::dirs()->saveLocation( "config" ) + "knotesrc";
        KSharedConfig::Ptr config = KSharedConfig::openConfig( configFile, false, false );
        m_defaultConfig = new KNoteConfig( config );
    }

    // create a new preferences dialog...
    KNoteConfigDlg *dialog = new KNoteConfigDlg( m_defaultConfig,
            i18n("Default Settings"), true, this, "KNotes Default Settings" );
    connect( dialog, SIGNAL(settingsChanged()), this, SLOT(updateNetworkListener()) );
    dialog->show();
}

void KNotesApp::slotConfigureAccels()
{
    KKeyDialog::configure( m_globalAccel, this, false );
    m_globalAccel->writeSettings();
    updateGlobalAccels();
}

void KNotesApp::slotNoteKilled( KCal::Journal *journal )
{
    // this kills the KNote object
    m_noteList.remove( journal->uid() );
    m_manager->deleteNote( journal );

    saveNotes();
    updateNoteActions();
}

void KNotesApp::slotQuit()
{
    saveConfigs();
    kapp->quit();
}


// -------------------- private methods -------------------- //

void KNotesApp::showNote( KNote* note ) const
{
    if ( !note->isHidden() )
    {
        // if it's already showing, we need to change to its desktop
        // and give it focus
        KWin::setCurrentDesktop( KWin::windowInfo( note->winId() ).desktop() );
        KWin::forceActiveWindow( note->winId() );
        note->setFocus();
    }
    else
    {
        // if not, show note on the current desktop
        note->show();
        note->toDesktop( KWin::currentDesktop() );
        KWin::forceActiveWindow( note->winId() );
        note->setFocus();
    }
}

void KNotesApp::createNote( KCal::Journal *journal )
{
    KNote *newNote = new KNote( this, domDocument(), journal,
                                0, journal->uid().utf8() );
    m_noteList.insert( newNote->noteId(), newNote );

    connect( newNote, SIGNAL(sigRequestNewNote()), SLOT(newNote()) );
    connect( newNote, SIGNAL(sigKillNote( KCal::Journal* )),
                        SLOT(slotNoteKilled( KCal::Journal* )) );
    connect( newNote, SIGNAL(sigNameChanged()), SLOT(updateNoteActions()) );
    connect( newNote, SIGNAL(sigDataChanged()), SLOT(saveNotes()) );
    connect( newNote, SIGNAL(sigColorChanged()), SLOT(updateNoteActions()) );

    updateNoteActions();
}

void KNotesApp::acceptConnection()
{
    // Accept the connection and make KNotesNetworkReceiver do the job
    KExtendedSocket *s;
    m_listener->accept( s );
    KNotesNetworkReceiver *recv = new KNotesNetworkReceiver( s );
    connect( recv, SIGNAL(sigNoteReceived( const QString &, const QString & )),
             this, SLOT(newNote( const QString &, const QString & )) );
}

void KNotesApp::saveNotes()
{
    m_manager->save();
}

void KNotesApp::saveConfigs()
{
    QDictIterator<KNote> it( m_noteList );
    for ( ; it.current(); ++it )
        it.current()->saveConfig();
}

void KNotesApp::updateNoteActions()
{
    unplugActionList( "notes" );
    m_noteActions.clear();

    for ( QDictIterator<KNote> it( m_noteList ); it.current(); ++it )
    {
        KAction *action = new KAction( it.current()->name().replace("&", "&&"),
                                       KShortcut(), this, SLOT(slotShowNote()),
                                       (QObject *)0,
                                       it.current()->noteId().utf8() );
        QPixmap pix( 16, 16 );
        pix.fill( it.current()->paletteBackgroundColor() );
        action->setIconSet( pix );
        m_noteActions.append( action );
    }

    m_noteActions.sort();

    if ( m_noteActions.isEmpty() )
    {
        KAction *action = new KAction( i18n("No Notes") );
        m_noteActions.append( action );
    }

    plugActionList( "notes", m_noteActions );
}

void KNotesApp::updateGlobalAccels()
{
    if ( m_globalAccel->isEnabled() )
    {
        KAction *action = actionCollection()->action( "new_note" );
        if ( action )
            action->setShortcut( m_globalAccel->shortcut( "global_new_note" ) );
        action = actionCollection()->action( "new_note_clipboard" );
        if ( action )
            action->setShortcut( m_globalAccel->shortcut( "global_new_note_clipboard" ) );

        m_globalAccel->updateConnections();
    }
    else
    {
        KAction *action = actionCollection()->action( "new_note" );
        if ( action )
            action->setShortcut( 0 );
        action = actionCollection()->action( "new_note_clipboard" );
        if ( action )
            action->setShortcut( 0 );
    }
}

void KNotesApp::updateNetworkListener()
{
    m_listener->reset();

    if ( KNotesGlobalConfig::receiveNotes() )
    {
        m_listener->setPort( KNotesGlobalConfig::port() );
        m_listener->listen();
    }
}

#include "knotesapp.moc"
