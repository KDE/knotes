/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2001, The KNotes Developers

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
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*******************************************************************/

#include <qdir.h>

#include <kapp.h>
#include <kwin.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <kpopupmenu.h>
#include <khelpmenu.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <ksimpleconfig.h>
#include <kio/netaccess.h>
#include <kurl.h>

#include <kaction.h>
#include <kstdaction.h>
#include <kxmlgui.h>

#include <unistd.h>

#include "knotesapp.h"
#include "knote.h"
#include "knoteconfigdlg.h"


KNotesApp::KNotesApp()
    : QLabel( 0, 0, WType_TopLevel ), DCOPObject("KNotesIface"),
      KXMLGUIBuilder( this )
{
    m_noteActions.setAutoDelete( true );

    // create the dock widget...
    KWin::setSystemTrayWindowFor( winId(), qt_xrootwin() );
    setBackgroundMode( X11ParentRelative );
    setPixmap( KGlobal::iconLoader()->loadIcon( "knotes", KIcon::Small ) );

    // create the GUI...
    new KAction( i18n("New Note"), "filenew", 0, this, SLOT(slotNewNote()), actionCollection(), "new_note" );
    new KAction( i18n("&Preferences..."), "configure", 0, this, SLOT(slotPreferences()), actionCollection(), "options_configure" );
    new KAction( i18n("&Quit"), "exit", 0, kapp, SLOT( closeAllWindows() ), actionCollection(), "quit" );
    new KHelpMenu( this, kapp->aboutData(), false, actionCollection() );

    setXMLFile( QString( instance()->instanceName() + "ui.rc" ) );
    setXMLGUIBuildDocument( QDomDocument() );

    factory = new KXMLGUIFactory( this, this, "guifactory" );
    factory->addClient( this );

    m_context_menu = static_cast<KPopupMenu*>(factory->container( "knotes_context", this ));
    m_note_menu = static_cast<KPopupMenu*>(factory->container( "notes_menu", this ));

    // remove old (KDE 1.x) local config file if it still exists
    QString configfile = KGlobal::dirs()->findResource( "config", "knotesrc" );
    KSimpleConfig *test = new KSimpleConfig( configfile, true );
    test->setGroup( "General" );
    if ( test->readNumEntry( "version", 1 ) == 1 )
    {
        delete test;
        if ( !( checkAccess( configfile, W_OK ) &&
                KIO::NetAccess::del( KURL(configfile) ) ) )
        {
            kdError() << "Could not delete old config file!!" << endl;
            // TODO
        }
    } else
        delete test;

    QDir noteDir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );

    // clean up note names (pre KNotes 2.1)
    QStringList notes = noteDir.entryList( QDir::Files, QDir::Name );   // this doesn't list the hidden data files
    for ( QStringList::Iterator n = notes.begin(); n != notes.end(); n++ )
    {
        bool number;
        (*n).mid( 6 ).toInt( &number );
        if ( !((*n).startsWith( "KNote " ) && number) )
        {
            QString newName;
            for ( int i = 1; ; i++ )
            {
                newName = QString( "KNote %1" ).arg(i);
                if ( !noteDir.exists( newName ) )
                    break;
            }
            noteDir.rename( *n, newName, false );
            noteDir.rename( "." + (*n) + "_data", "." + newName + "_data", false );
            kdDebug() << "Note " << *n << " renamed to " << newName << endl;
        }
    }

    // now read the notes
    notes = noteDir.entryList( QDir::Files, QDir::Name );   // this doesn't list the hidden data files
    for ( QStringList::Iterator i = notes.begin(); i != notes.end(); i++ )
    {
        KNote* newNote = new KNote( *i, true );

        connect( newNote, SIGNAL( sigRenamed(const QString&, const QString&) ),
                 this,    SLOT( slotNoteRenamed(const QString&, const QString&) ) );
        connect( newNote, SIGNAL( sigNewNote() ),
                 this,    SLOT( slotNewNote() ) );
        connect( newNote, SIGNAL( sigKilled(const QString&) ),
                 this,    SLOT( slotNoteKilled(const QString&) ) );

        m_noteList.insert( newNote->name(), newNote );
    }
    updateNoteActions();

    kapp->installEventFilter( this );

    if ( m_noteList.count() == 0 && !kapp->isRestored() )
        slotNewNote();
}

KNotesApp::~KNotesApp()
{
    delete m_note_menu;
    delete m_context_menu;
    m_noteList.clear();
    delete factory;
}

bool KNotesApp::saveState( QSessionManager& )
{
    saveNotes();

    QDictIterator<KNote> it( m_noteList );
    for ( ; it.current(); ++it )
        it.current()->hide();

   return true;
}

bool KNotesApp::commitData( QSessionManager& )
{
    saveNotes();
    return true;
}


// -------------------- public DCOP interface -------------------- //

int KNotesApp::newNote( QString name, const QString& text )
{
    if ( !name.isNull() && m_noteList[name] )
    {
        kdError() << "A note with this name already exists!" << endl;
        return -1;
    }
    
    // must be done here to check if !m_noteList[name]
    QDir noteDir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
    if ( name.isEmpty() )
    {
        for ( int i = 1; ; i++ )
        {
            name = QString( "KNote %1" ).arg(i);
            if ( !m_noteList[name] && !noteDir.exists( name ) )
                break;
        }
    }

    KNote* newNote = new KNote( name );
    newNote->setText( text );

    connect( newNote, SIGNAL( sigRenamed(const QString&, const QString&) ),
             this,    SLOT( slotNoteRenamed(const QString&, const QString&) ) );
    connect( newNote, SIGNAL( sigNewNote() ),
             this,    SLOT( slotNewNote() ) );
    connect( newNote, SIGNAL( sigKilled(const QString&) ),
             this,    SLOT( slotNoteKilled(const QString&) ) );

    m_noteList.insert( newNote->name(), newNote );
    updateNoteActions();

    return newNote->noteId();
}

void KNotesApp::showNote( const QString& name ) const
{
    KNote* note = m_noteList[name];

    if ( !note )
    {
        kdWarning() << "No note named " << name << endl;
        return;
    }

    showNote( note );
}

void KNotesApp::showNote( int noteId ) const
{
    KNote* note = noteById( noteId );
    
    if ( !note )
    {
        kdWarning() << "No note with id " << noteId << endl;
        return;
    }

    showNote( note );
}

void KNotesApp::killNote( const QString& name )
{
    KNote* note = m_noteList[name];
    if ( note )
        note->slotKill();
}

void KNotesApp::killNote( int noteId )
{
    KNote* note = noteById( noteId );
    if ( note )
        note->slotKill();
}

QMap<int,QString> KNotesApp::notes() const
{
    QMap<int,QString> notes;
    QDictIterator<KNote> it( m_noteList );

    for ( ; it.current(); ++it )
        notes.insert( it.current()->noteId(), it.current()->name() );

    return notes;
}

QString KNotesApp::text( const QString& name ) const
{
    KNote* note = m_noteList[name];
    
    if ( note )
        return note->text();
    else
        return QString::null;
}

QString KNotesApp::text( int noteId ) const
{
    KNote* note = noteById( noteId );
    
    if ( note )
        return note->text();
    else
        return QString::null;
}

void KNotesApp::setName( const QString& oldName, const QString& newName )
{
    slotNoteRenamed( oldName, newName );
}

void KNotesApp::setName( int noteId, const QString& newName )
{
    KNote* note = noteById( noteId );
    if ( note )
        slotNoteRenamed( note->name(), newName );
}

void KNotesApp::setText( const QString& name, const QString& newText )
{
    KNote* note = m_noteList[name];
    if ( note )
        note->setText( newText );
}

void KNotesApp::setText( int noteId, const QString& newText )
{
    KNote* note = noteById( noteId );
    if ( note )
        note->setText( newText );
}

void KNotesApp::sync( const QString& app )
{
    QDictIterator<KNote> it( m_noteList );

    for ( ; it.current(); ++it )
        it.current()->sync( app );
}

bool KNotesApp::isNew( const QString& app, const QString& name ) const
{
    KNote* note = m_noteList[name];

    if ( note )
        return note->isNew( app );
    else
        return false;
}

bool KNotesApp::isNew( const QString& app, int noteId ) const
{
    KNote* note = noteById( noteId );

    if ( note )
        return note->isNew( app );
    else
        return false;
}

bool KNotesApp::isModified( const QString& app, const QString& name ) const
{
    KNote* note = m_noteList[name];

    if ( note )
        return note->isModified( app );
    else
        return false;
}

bool KNotesApp::isModified( const QString& app, int noteId ) const
{
    KNote* note = noteById( noteId );

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
        if ( m_note_menu->count() > 0 )
            m_note_menu->popup( e->globalPos() );
        break;
    case MidButton:
    case RightButton:
        m_context_menu->popup( e->globalPos() );
        break;
    default:
        break;
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
            KNote* first = it.current();
            for ( ; it.current(); ++it )
                if ( it.current()->hasFocus() ) {
                    if ( ++it )
                        showNote( it.current() );
                    else
                        showNote( first );
                    break;
                }

            ke->accept();
            return true;
        }
    }

    return QObject::eventFilter( o, ev );
}


// -------------------- protected slots -------------------- //

void KNotesApp::slotNewNote()
{
    newNote();
}

void KNotesApp::slotShowNote()
{
    //tell the WM to give this note focus
    QString name = QString::fromUtf8( sender()->name() );
    showNote( name );
}

void KNotesApp::slotNoteRenamed( const QString& oldname, const QString& newname )
{
    if ( m_noteList[newname] )
    {
        KMessageBox::sorry( this, i18n("There is already a note with that name") );
        return;
    }

    KNote* note = m_noteList[oldname];
    if ( note )
    {
        m_noteList.remove( oldname );
        m_noteList.insert( newname, note );
        note->setName( newname );

        updateNoteActions();
    }
    else
        kdError() << "There is no note named: " << oldname << endl;
}

void KNotesApp::slotNoteKilled( const QString& name )
{
    m_noteList.remove( name );
    updateNoteActions();
}

void KNotesApp::slotPreferences() const
{
    //launch preferences dialog...
    KNoteConfigDlg config( "knotesrc", i18n("KNotes Defaults") );
    config.exec();
}


// -------------------- private methods -------------------- //

KNote* KNotesApp::noteById( int noteId ) const
{
    QDictIterator<KNote> it( m_noteList );

    for ( ; it.current(); ++it )
        if ( it.current()->noteId() == noteId )
            return it.current();

    return 0L;
}

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
        // if not, show note on the current desktop
        note->show();
        note->slotToDesktop( KWin::currentDesktop() );
        KWin::setActiveWindow( note->winId() );
        note->setFocus();
    }
}

void KNotesApp::saveNotes() const
{
    // save all the notes...
    QDictIterator<KNote> it( m_noteList );
    for ( ; it.current(); ++it )
    {
        it.current()->saveData();
        it.current()->saveConfig();
        it.current()->saveDisplayConfig();
    }
}

void KNotesApp::updateNoteActions()
{
    unplugActionList( "notes" );
    m_noteActions.clear();

    for ( QDictIterator<KNote> it( m_noteList ); it.current(); ++it )
    {
        KAction *action = new KAction( it.currentKey(), 0, 0, it.currentKey().utf8() );
        connect( action, SIGNAL( activated() ), this, SLOT( slotShowNote() ) );
        m_noteActions.append( action );
    }

    if ( !m_noteActions.isEmpty() )
        plugActionList( "notes", m_noteActions );
}

#include "knotesapp.moc"
