/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (C) Bernd Johannes Wuebben
     wuebben@math.cornell.edu
     wuebben@kde.org

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

#include "knotesapp.h"
#include "knoteconfigdlg.h"

#include <kapp.h>
#include <kwin.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <ksimpleconfig.h>
#include <kio/netaccess.h>
#include <kurl.h>

#include <qdir.h>
#include <qfont.h>

KNotesApp::KNotesApp()
    : KSystemTray(),
	  DCOPObject("KNotesDCOP")
{
    //create the dock widget....
    setPixmap( KGlobal::iconLoader()->loadIcon( "knotes", KIcon::Small ) );

    m_note_menu = new KPopupMenu( this );
    connect( m_note_menu, SIGNAL( aboutToShow() ),
             this,        SLOT( slotPrepareNoteMenu() ) );
    connect( m_note_menu, SIGNAL( activated(int) ),
             this,        SLOT( slotToNote(int) ) );

    KPopupMenu* menu = contextMenu();
    menu->insertItem( i18n("New Note"), this, SLOT(slotNewNote(int)) );
    menu->insertItem( i18n("Preferences..."), this, SLOT(slotPreferences(int)) );
    menu->insertItem( i18n("Notes"), m_note_menu );

    loadNotes();

    if( m_NoteList.count() == 0 && !kapp->isRestored() )
        slotNewNote();
}


KNotesApp::~KNotesApp()
{
    delete m_note_menu;
    m_NoteList.clear();
}

void KNotesApp::loadNotes()
{
    QString str_notedir = KGlobal::dirs()->saveLocation( "appdata", "notes/" );
    QDir notedir( str_notedir );
    QStringList notes = notedir.entryList( QDir::Files, QDir::Name ); //doesn't list hidden files

    for( QStringList::Iterator i = notes.begin(); i != notes.end(); ++i )
    {
        //check to see if this note is already shown...
        if( m_NoteList[ *i ] )
            continue;

        newNote( *i );
    }
}

/* virtual */ ASYNC KNotesApp::showNote( QString name )
{
    kdWarning() << "this isn't implemented yet" << endl;
}

ASYNC KNotesApp::rereadNotesDir()
{
    loadNotes();
}

ASYNC KNotesApp::addNote( QString title, QString body,
                          unsigned long pilotID )
{
    newNote( title, body );

	KNote* tmp = m_NoteList[title];
	if( tmp )
	{
	    QDir appdir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
        QString sc_filename = appdir.absFilePath( title );

        KSimpleConfig sc( sc_filename );
        sc.setGroup( "KPilot" );
        sc.writeEntry( "pilotID", pilotID );
        sc.sync();
	}
	else
	{
	    kdError() << "could not add note via dcop: " << title << endl;
	}
}

void KNotesApp::slotNewNote( int /*id*/ )
{
    newNote();
}

void KNotesApp::newNote( const QString& note_name, const QString& text )
{
    QDir appdir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
    QString thename;

    //handle cases for the name
    if( note_name.isEmpty() )
    {
        //find a suitable name
        for( int i = 1; ; i++ )
        {
            thename = QString( "KNote %1" ).arg(i);
            if( !appdir.exists( thename ) )
                break;
        }
    }
    else
    {
        thename = note_name;
        if( m_NoteList[thename] )
        {
            kdError() << "This note is already showing" << endl;
            return;
        }
    }

    //handle the cases for if there is already a config file...
    QString config = appdir.absFilePath( thename );
    if( !appdir.exists( thename ) )
    {
        KIO::NetAccess::copy( KURL( KGlobal::dirs()->findResource( "config", "knotesrc" ) ),
                              KURL( config ) );
    }

    KSimpleConfig sc( config );
    sc.setGroup( "Data" );
    if( sc.readEntry( "name" ) != thename )
    {
        sc.writeEntry( "name", thename );
        sc.sync();
    }

    sc.setGroup( "General" );
    int version = sc.readNumEntry( "version", 1 );

    KNote* newnote = new KNote( config, version == 1 );
    if( !text.isEmpty() )
        newnote->setText( text );

    connect( newnote, SIGNAL( sigRenamed(QString&, QString&) ),
             this,    SLOT( slotNoteRenamed(QString&, QString&) ) );
    connect( newnote, SIGNAL( sigNewNote(int) ),
             this,    SLOT( slotNewNote(int) ) );
    connect( newnote, SIGNAL( sigKilled(QString) ),
             this,    SLOT( slotNoteKilled(QString) ) );

    m_NoteList.insert( thename, newnote );
}

void KNotesApp::slotNoteRenamed( QString& oldname, QString& newname )
{
    KNote* tmp = m_NoteList[oldname];
    m_NoteList.insert( newname, tmp );
    m_NoteList.remove( oldname );
}

void KNotesApp::slotNoteKilled( QString name )
{
    m_NoteList.remove( name );
}

void KNotesApp::slotPreferences( int )
{
    //launch preferences dialog...
    KNoteConfigDlg tmpconfig( "knotesrc", i18n("KNotes Defaults") );
    tmpconfig.exec();
}

void KNotesApp::slotToNote( int id )
{
    //tell the WM to give this note focus
    QString name = m_note_menu->text( id );

    //if it's already showing, we need to change to its desktop
    //and give it focus
    KNote* tmpnote = m_NoteList[name];
    if( !tmpnote->isHidden() )
    {
        KWin::setActiveWindow(tmpnote->winId());
        tmpnote->setFocus();
    }
    else
    {
        //if not show it on the current desktop
        tmpnote->show();
        tmpnote->slotToDesktop( KWin::currentDesktop() );
        KWin::setActiveWindow(tmpnote->winId());
        tmpnote->setFocus();
    }
}

void KNotesApp::slotPrepareNoteMenu()
{
    //find all notes- get their names and put them into the menu
    m_note_menu->clear();

    QDictIterator<KNote> it( m_NoteList );
    int id = 0;
    while( it.current() )
    {
        m_note_menu->insertItem( it.currentKey() );
        ++id;
        ++it;
    }
}

void KNotesApp::slotSaveNotes()
{
    //save all the notes...
    QDictIterator<KNote> it( m_NoteList );
    for( ; it.current(); ++it )
    {
        it.current()->saveData();
        it.current()->saveConfig();
        it.current()->saveDisplayConfig();
    }
}

void KNotesApp::mouseReleaseEvent( QMouseEvent * e)
{
    if ( rect().contains( e->pos() ) && e->button() == LeftButton )
    {
        slotPrepareNoteMenu();
        if( m_note_menu->count() > 0 )
            m_note_menu->popup( e->globalPos() );
        return;
    }
}


#include "knotesapp.moc"
