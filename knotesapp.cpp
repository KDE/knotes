/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2003, The KNotes Developers

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
#include <qdir.h>

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
#include <kio/netaccess.h>
#include <kwin.h>

#include "knotesapp.h"
#include "knote.h"
#include "knoteconfigdlg.h"
#include "knoteslegacy.h"
#include "resourcemanager.h"
#include "version.h"


int KNotesApp::KNoteActionList::compareItems( QPtrCollection::Item s1, QPtrCollection::Item s2 )
{ 
    if ( ((KAction*)s1)->text() == ((KAction*)s2)->text() ) 
        return 0; 
    return ( ((KAction*)s1)->text() < ((KAction*)s2)->text() ? -1 : 1 ); 
}


KNotesApp::KNotesApp()
    : DCOPObject("KNotesIface"), QLabel( 0, 0, WType_TopLevel ),
      KXMLGUIBuilder( this )
{
    m_noteActions.setAutoDelete( true );

    // create the dock widget...
    KWin::setSystemTrayWindowFor( winId(), qt_xrootwin() );
    setBackgroundMode( X11ParentRelative );
    setPixmap( KSystemTray::loadIcon( "knotes" ) );

    // create the GUI...
    new KAction( i18n("New Note"), "filenew", 0, 
        this, SLOT(slotNewNote()), actionCollection(), "new_note" );
    new KAction( i18n("New Note From Clipboard"), "editpaste", 0, 
        this, SLOT(slotNewNoteFromClipboard()), actionCollection(), "new_note_clipboard" );
    new KHelpMenu( this, kapp->aboutData(), false, actionCollection() );

    KStdAction::preferences( this, SLOT(slotPreferences()), actionCollection() );
    KStdAction::keyBindings( this, SLOT(slotConfigureAccels()), actionCollection() );
    KStdAction::quit( this, SLOT(slotQuit()), actionCollection() )->setShortcut( 0 );

    setXMLFile( QString( instance()->instanceName() + "ui.rc" ) );
    factory = new KXMLGUIFactory( this, this, "guifactory" );
    factory->addClient( this );

    m_context_menu = static_cast<KPopupMenu*>(factory->container( "knotes_context", this ));
    m_note_menu = static_cast<KPopupMenu*>(factory->container( "notes_menu", this ));

    // create accels for global shortcuts
    globalAccel = new KGlobalAccel( this, "global accel" );
    globalAccel->insert( "global_new_note", i18n("New Note"), "",
                         ALT+SHIFT+Key_N, ALT+SHIFT+Key_N ,
                         this, SLOT(slotNewNote()), true, true );
    globalAccel->insert( "global_new_note_clipboard", i18n("New Note From Clipboard"), "",
                         ALT+SHIFT+Key_C, ALT+SHIFT+Key_C,
                         this, SLOT(slotNewNoteFromClipboard()), true, true );

    globalAccel->readSettings();

    KConfig *config = KGlobal::config();
    config->setGroup( "Global Keybindings" );
    globalAccel->setEnabled( config->readBoolEntry( "Enabled", true ) );

    updateGlobalAccels();

    // clean up old config files
    KNotesLegacy::cleanUp();

    // read the notes
    m_manager = new ResourceManager( this );
    m_manager->load();
    updateNoteActions();

    connect( kapp, SIGNAL(lastWindowClosed()), kapp, SLOT(quit()) );

    kapp->installEventFilter( this );

    if ( m_manager->count() == 0 && !kapp->isRestored() )
        newNote();
}

KNotesApp::~KNotesApp()
{
    saveNotes();
    delete factory;
}

bool KNotesApp::commitData( QSessionManager& )
{
    saveConfig();
    return true;
}

// -------------------- public DCOP interface -------------------- //

QString KNotesApp::newNote( const QString& name, const QString& text )
{
    return m_manager->newNote( name, text );
}

QString KNotesApp::newNoteFromClipboard( const QString& name )
{
    const QString& text = KApplication::clipboard()->text();
    return newNote( name, text );
}

void KNotesApp::showNote( const QString& id ) const
{
    KNote* note = m_manager->note( id );
    if ( note )
        showNote( note );
    else
        kdWarning(5500) << "No note with id: " << id << endl;
}

void KNotesApp::hideNote( const QString& id ) const
{
    KNote* note = m_manager->note( id );
    if ( note )
        note->hide();
    else
        kdWarning(5500) << "No note with id: " << id << endl;
}

void KNotesApp::killNote( const QString& id, bool force ) 
{
    KNote* note = m_manager->note( id );
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
    return m_manager->notes();
}

QString KNotesApp::name( const QString& id ) const
{
    KNote* note = m_manager->note( id );
    if ( note )
        return note->name();
    else
        return QString::null;
}

QString KNotesApp::text( const QString& id ) const
{
    KNote* note = m_manager->note( id );
    if ( note )
        return note->text();
    else
        return QString::null;
}

void KNotesApp::setName( const QString& id, const QString& newName )
{
    KNote* note = m_manager->note( id );
    if ( note )
        note->setName( newName );
    else
        kdWarning(5500) << "No note with id: " << id << endl;
}

void KNotesApp::setText( const QString& id, const QString& newText )
{
    KNote* note = m_manager->note( id );
    if ( note )
        note->setText( newText );
    else
        kdWarning(5500) << "No note with id: " << id << endl;
}

void KNotesApp::sync( const QString& app )
{
    m_manager->sync( app );
}

bool KNotesApp::isNew( const QString& app, const QString& id ) const
{
    KNote* note = m_manager->note( id );
    if ( note )
        return note->isNew( app );
    else
        return false;
}

bool KNotesApp::isModified( const QString& app, const QString& id ) const
{
    KNote* note = m_manager->note( id );
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
        if ( m_manager->count() == 1 )
        {
            showNote( m_manager->first() );
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
            m_manager->showNextNote();
            ke->accept();
            return true;
        }
        else
            ke->ignore();
    }

    return QLabel::eventFilter( o, ev );
}


// -------------------- protected slots -------------------- //

void KNotesApp::slotNewNote()
{
    newNote();
}

void KNotesApp::slotNewNoteFromClipboard()
{
    newNoteFromClipboard();
}

void KNotesApp::slotShowNote()
{
    // tell the WM to give this note focus
    QString name = QString::fromUtf8( sender()->name() );
    showNote( name );
}

void KNotesApp::slotPreferences() const
{
    // launch preferences dialog...
    KNoteConfigDlg config( "knotesrc", i18n("KNotes Defaults"), true );
    config.exec();
}

void KNotesApp::slotConfigureAccels()
{
    KKeyDialog::configure( globalAccel, this, false );
    globalAccel->writeSettings();
    updateGlobalAccels();
}

void KNotesApp::slotQuit()
{
    saveConfig();
    kapp->quit();
}


// -------------------- private methods -------------------- //

void KNotesApp::showNote( KNote* note ) const
{
    if ( !note->isHidden() )
    {
        // if it's already showing, we need to change to its desktop
        // and give it focus
        KWin::setCurrentDesktop( KWin::info( note->winId() ).desktop );
        KWin::setActiveWindow( note->winId() );
        note->setFocus();
    }
    else
    {
        WId id = note->winId();

        // if not, show note on the current desktop
        note->show();
        note->toDesktop( KWin::currentDesktop() );
        if ( note->m_alwaysOnTop->isChecked() )
            KWin::setState( id, KWin::info( id ).state | NET::StaysOnTop );
        KWin::setActiveWindow( id );
        note->setFocus();
    }
}

void KNotesApp::saveConfig()
{
    m_manager->saveConfigs();
}

void KNotesApp::saveNotes()
{
    m_manager->save();
}

void KNotesApp::updateNoteActions()
{
    unplugActionList( "notes" );
    m_noteActions.clear();

    for ( QDictIterator<KNote> it = m_manager->iterator(); it.current(); ++it )
    {
        KAction *action = new KAction( it.current()->name().replace("&", "&&"),
                                       KShortcut(), this, SLOT(slotShowNote()),
                                       (QObject*)0,
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
    if ( globalAccel->isEnabled() )
    {
        KAction *action = actionCollection()->action( "new_note" );
        if ( action )
            action->setShortcut( globalAccel->shortcut( "global_new_note" ) );
        action = actionCollection()->action( "new_note_clipboard" );
        if ( action )
            action->setShortcut( globalAccel->shortcut( "global_new_note_clipboard" ) );

        globalAccel->updateConnections();
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

#include "knotesapp.moc"
