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
#include <kmessagebox.h>
#include <ksimpleconfig.h>
#include <kio/netaccess.h>
#include <kwin.h>

#include "libkcal/journal.h"
#include "libkcal/icalformat.h"

#include "knotesapp.h"
#include "knote.h"
#include "knoteconfigdlg.h"
#include "knoteslegacy.h"
#include "version.h"

using namespace KCal;


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
    m_noteList.setAutoDelete( true );

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
    m_calendar.load( KGlobal::dirs()->saveLocation( "appdata" ) + "notes.ics" );

    // TODO
    // initialize the Calendar
    //m_calendar.setOwner(..);
    //m_calendar.setEmail(..);

    // read the old config files into m_calendar and convert them
    if ( KNotesLegacy::convert( &m_calendar ) )
        saveNotes();

    QDir noteDir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
    Journal::List notes = m_calendar.journals();
    Journal::List::ConstIterator it;
    for ( it = notes.begin(); it != notes.end(); ++it )
    {
        Journal *note = *it;
        // KOrganizers journals don't have attachments -> use default display config
        if ( note->attachments(CONFIG_MIME).isEmpty() )
        {
            // set the name of the config file...
            QString file = noteDir.absFilePath( note->uid() );

            // ...and "fill" it with the default config
            KIO::NetAccess::copy(
                    KURL( KGlobal::dirs()->findResource( "config", "knotesrc" ) ),
                    KURL( file )
            );

            note->addAttachment( new Attachment( file, CONFIG_MIME ) );
        }

        if ( note->summary().isNull() && note->dtStart().isValid() )
            note->setSummary( KGlobal::locale()->formatDateTime( note->dtStart() ) );

        KNote* newNote = new KNote( this, domDocument(), note );
        m_noteList.insert( note->uid(), newNote );

        connect( newNote, SIGNAL(sigNewNote()), this, SLOT(slotNewNote()) );
        connect( newNote, SIGNAL(sigKillNote( KCal::Journal* )),
                 this,    SLOT(slotNoteKilled( KCal::Journal* )) );
        connect( newNote, SIGNAL(sigNameChanged()), this, SLOT(updateNoteActions()) );
        connect( newNote, SIGNAL(sigSaveData()), this, SLOT(saveNotes()) );
    }
    updateNoteActions();

    connect( kapp, SIGNAL(lastWindowClosed()), kapp, SLOT(quit()) );

    kapp->installEventFilter( this );

    if ( m_noteList.count() == 0 && !kapp->isRestored() )
        newNote();
}

KNotesApp::~KNotesApp()
{
    saveNotes();

    blockSignals( true );
    m_noteList.clear();
    blockSignals( false );

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
    // create the new note
    Journal *note = new Journal();

    // set the name of the config file...
    QDir noteDir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
    QString file = noteDir.absFilePath( note->uid() );

    // ...and "fill" it with the default config
    KIO::NetAccess::copy( KURL( KGlobal::dirs()->findResource( "config", "knotesrc" ) ),
                          KURL( file ) );

    // new notes have the current date/time as title if none was given
    if ( !name.isNull() )
        note->setSummary( name );
    else
        note->setSummary( KGlobal::locale()->formatDateTime( QDateTime::currentDateTime() ) );

    note->setDescription( text );
    note->addAttachment( new Attachment( file, CONFIG_MIME ) );
    m_calendar.addJournal( note );

    KNote* newNote = new KNote( this, domDocument(), note );
    m_noteList.insert( newNote->noteId(), newNote );

    connect( newNote, SIGNAL(sigNewNote()), this, SLOT(slotNewNote()) );
    connect( newNote, SIGNAL(sigKillNote( KCal::Journal* )),
             this,    SLOT(slotNoteKilled( KCal::Journal* )) );
    connect( newNote, SIGNAL(sigNameChanged()), this, SLOT(updateNoteActions()) );
    connect( newNote, SIGNAL(sigSaveData()), this, SLOT(saveNotes()) );

    updateNoteActions();
    showNote( newNote );

    return newNote->noteId();
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

void KNotesApp::killNote( const QString& id ) // "bool force = false" doesn't work with dcop 
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
            KNote* first = it.toFirst();
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

void KNotesApp::slotNoteKilled( Journal *journal )
{
    // this kills the KNote object
    m_noteList.remove( journal->uid() );

    QString configFile = journal->attachments( CONFIG_MIME ).first()->uri();
    if ( !QDir::home().remove( configFile ) )
        kdError(5500) << "Can't remove the note config: " << configFile << endl;

    m_calendar.deleteJournal( journal );

    updateNoteActions();
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
    QDictIterator<KNote> it( m_noteList );
    for ( ; it.current(); ++it )
        it.current()->saveConfig();
}

void KNotesApp::saveNotes()
{
    ICalFormat format;

    QString file = KGlobal::dirs()->saveLocation( "appdata" ) + "notes.ics";
    QString backup = file + "~";

    // if the backup fails don't even try to save the current notes
    // (might just destroy the file that's already there)

    if ( KIO::NetAccess::exists( KURL( file ) ) && !KIO::NetAccess::file_copy( KURL( file ), KURL( backup ), -1, true) )
        KMessageBox::error(0, i18n("<qt>Unable to save the notes backup to <b>%1</b>! Check that there is sufficient disk space.</qt>")
                                  .arg( backup ) );
    else if ( !m_calendar.save( file, &format ) )
        KMessageBox::error(0, i18n("<qt>Unable to save the notes to <b>%1</b>! Check that there is sufficient disk space.<br>"
                                   "There should be a backup in <b>%2</b> though.</qt>")
                                  .arg( file ).arg( backup ) );
}

void KNotesApp::updateNoteActions()
{
    unplugActionList( "notes" );
    m_noteActions.clear();

    for ( QDictIterator<KNote> it( m_noteList ); it.current(); ++it )
    {
        KAction *action = new KAction( it.current()->name().replace( "&", "&&"), KShortcut(),
                                       this, SLOT(slotShowNote()),
                                       (QObject*)0, it.current()->noteId().utf8() );
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
