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

#include <qdir.h>
#include <qfont.h>

KNotesApp::KNotesApp()
    : KSystemTray()
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

    //initialize saved notes, if none create a note...
    QString str_notedir = KGlobal::dirs()->saveLocation( "appdata", "notes/" );
    QDir notedir( str_notedir );
    QStringList notes = notedir.entryList( QDir::Files, QDir::Name ); //doesn't list hidden files

    for( QStringList::Iterator i = notes.begin(); i != notes.end(); ++i )
    {
        QString configfile = notedir.absFilePath( *i );
        KSimpleConfig* tmp = new KSimpleConfig( configfile );
        tmp->setGroup( "General" );
        int version = tmp->readNumEntry( "version", 1 );
        delete tmp;

        KNote* tmpnote = new KNote( configfile, version == 1 );

        connect( tmpnote, SIGNAL( sigRenamed(QString&, QString&) ),
                 this,    SLOT  ( slotNoteRenamed(QString&, QString&) ) );
        connect( tmpnote, SIGNAL( sigNewNote(int) ),
                 this,    SLOT  ( slotNewNote(int) ) );
        connect( tmpnote, SIGNAL( sigKilled(QString) ),
                 this,    SLOT  ( slotNoteKilled(QString) ) );
        m_NoteList.insert( tmpnote->getName(), tmpnote );
    }

    if( m_NoteList.count() == 0 && !kapp->isRestored() )
        slotNewNote();

    setBackgroundMode(X11ParentRelative);
}


KNotesApp::~KNotesApp()
{
    delete m_note_menu;
    m_NoteList.clear();
}

void KNotesApp::copyDefaultConfig( QString& sc_filename, QString& newname )
{
    QString defaultsfile = KGlobal::dirs()->findResource( "config", "knotesrc" );
    KSimpleConfig defaults( defaultsfile );
    KSimpleConfig sc( sc_filename );

    sc.setGroup( "Display" );
    defaults.setGroup( "Display" );

    uint width  = defaults.readUnsignedNumEntry( "width", 200 );
    uint height = defaults.readUnsignedNumEntry( "height", 200 );
    QColor bgc  = defaults.readColorEntry( "bgcolor", &(Qt::yellow) );
    QColor fgc  = defaults.readColorEntry( "fgcolor", &(Qt::black) );

    sc.writeEntry( "width", width );
    sc.writeEntry( "height", height );
    sc.writeEntry( "bgcolor", bgc );
    sc.writeEntry( "fgcolor", fgc );

    sc.setGroup( "Editor" );
    defaults.setGroup( "Editor" );

    uint tabsize = defaults.readUnsignedNumEntry( "tabsize", 4 );
    bool indent  = defaults.readBoolEntry( "autoindent", true );

    sc.writeEntry( "tabsize", tabsize );
    sc.writeEntry( "autoindent", indent );

    QFont def_font( "helvetica" );
    QFont currfont = defaults.readFontEntry( "font", &def_font );
    sc.writeEntry( "font", currfont );

    sc.setGroup( "Actions" );
    defaults.setGroup( "Actions" );

    QString mailstr  = defaults.readEntry( "mail", "kmail --msg %f" );
    QString printstr = defaults.readEntry( "print", "a2ps -P %p -1 --center-title=%t --underlay=KDE %f" );

    sc.writeEntry( "mail", mailstr );
    sc.writeEntry( "print", printstr );

    sc.setGroup( "General" );
    sc.writeEntry( "version", 2 );

    sc.setGroup( "Data" );
    sc.writeEntry( "name", newname );

    // TODO: write default entries for the group "WindowDisplay"

    sc.sync();
}

void KNotesApp::slotNewNote( int /*id*/ )
{
    QString datadir = KGlobal::dirs()->saveLocation( "appdata", "notes/" );

    //find a new appropriate id for the new note...
    QString thename, configfile;
    QDir appdir( datadir );

    for( int i = 1; ; i++ )
    {
        thename = QString( "KNote %1" ).arg(i);

        if( !appdir.exists( thename ) )
        {
            configfile = appdir.absFilePath( thename );
            break;
        }
    }

    copyDefaultConfig( configfile, thename );

    KNote* newnote = new KNote( configfile );
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
