

#include "knotesapp.h"
#include "knoteconfigdlg.h"

#include <kapp.h>
#include <kwin.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>

KNotesApp::KNotesApp()
    : KSystemTray()
{
    // make sure the KNote objects get deleted
    //m_NoteList.setAutoDelete(true);

    //make sure I copy over the knotesrc to a local/writeable file- some
    QString globalConfigFile = KGlobal::dirs()->findResource( "config", "knotesrc" );
    m_defaults = new KConfig( globalConfigFile );

    //create the dock widget....
    setPixmap( KGlobal::iconLoader()->loadIcon( "knotes", KIcon::Small ) );
    //setPixmap( UserIcon( "knotes" ) );
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
    QStringList notes = notedir.entryList( "*" );

    int count = 0;
    for( QStringList::Iterator i = notes.begin(); i != notes.end(); ++i )
    {
        if( *i != "." && *i != ".." ) //ignore these
        {
            QString configfile = notedir.absFilePath( *i );
            KSimpleConfig* tmp = new KSimpleConfig( configfile );
            tmp->setGroup( "General" );
            int version = tmp->readNumEntry( "version", 1 );
            if( version > 1 )
            {
                KNote* tmpnote = new KNote( tmp );

                connect( tmpnote, SIGNAL( sigRenamed(QString&, QString&) ),
                         this,    SLOT  ( slotNoteRenamed(QString&, QString&) ) );
                connect( tmpnote, SIGNAL( sigNewNote(int) ),
                         this,    SLOT  ( slotNewNote(int) ) );
                connect( tmpnote, SIGNAL( sigKilled(QString) ),
                         this,    SLOT  ( slotNoteKilled(QString) ) );

                m_NoteList.insert( tmpnote->getName() ,tmpnote );
                tmpnote->show();
                ++count;
            }
            else
            {
                /*********************************************************/
                /*** PUT IN SUPPORT FOR READING THE OLD KNOTE FORMAT *****/
                /*********************************************************/
                kdDebug() << "This is an old note version, we can't read it yet" << endl;
            }
        }
    }

    if( count == 0 )
        slotNewNote();
}


KNotesApp::~KNotesApp()
{
    delete m_defaults;

}

void KNotesApp::copyDefaultConfig( KSimpleConfig* sc )
{
    sc->setGroup( "Display" );
    m_defaults->setGroup( "Display" );

    uint width = m_defaults->readUnsignedNumEntry( "width", 200 );
    uint height = m_defaults->readUnsignedNumEntry( "height", 200 );
    QColor bgc = m_defaults->readColorEntry( "bgcolor", &(Qt::yellow) );
    QColor fgc = m_defaults->readColorEntry( "fgcolor", &(Qt::black) );

    sc->writeEntry( "width", width );
    sc->writeEntry( "height", height );
    sc->writeEntry( "bgcolor", bgc );
    sc->writeEntry( "fgcolor", fgc );

    sc->setGroup( "Editor" );
    m_defaults->setGroup( "Editor" );

    uint tabsize = m_defaults->readUnsignedNumEntry( "tabsize", 4 );
    bool indent = m_defaults->readBoolEntry( "autoindent", true );

    sc->writeEntry( "tabsize", tabsize );
    sc->writeEntry( "autoindent", indent );

    sc->setGroup( "Actions" );
    m_defaults->setGroup( "Actions" );

    QString mailstr = m_defaults->readEntry( "mail", "kmail --msg %f" );
    QString printstr = m_defaults->readEntry( "print", "a2ps -P %p -1 --center-title=%t --underlay=KDE %f" );

    sc->writeEntry( "mail", mailstr );
    sc->writeEntry( "print", printstr );

    sc->setGroup( "General" );
    sc->writeEntry( "version", 2 );
}

void KNotesApp::slotNewNote( int id )
{
    QString datadir = KGlobal::dirs()->saveLocation( "appdata", "notes/" );

    //find a new appropriate id for the new note...
    bool exists;
    QString thename;
    QDir appdir( datadir );
    for( int i = 1; i < 51; i++ )   //set the unjust limit to 50 notes...
    {
        thename = QString( "KNote %1" ).arg(i);
        exists = false;

        if( !appdir.exists( thename ) )
        {
            exists = false;
            break;
        }
    }

    if( exists )
    {
        QString msg = i18n(""
        "You have exeeded the arbitrary and unjustly set limit of 50 knotes.\n"
        "Please complain to the author.");
        KMessageBox::sorry( NULL, msg );
        return;
    }

    KSimpleConfig* newconfig = new KSimpleConfig( datadir + thename );
    copyDefaultConfig( newconfig );
    newconfig->setGroup( "Data" );
    newconfig->writeEntry("name", thename );

    KNote* newnote = new KNote( newconfig );
    connect( newnote, SIGNAL( sigRenamed(QString&, QString&) ),
             this,    SLOT( slotNoteRenamed(QString&, QString&) ) );
    connect( newnote, SIGNAL( sigNewNote(int) ),
             this,    SLOT( slotNewNote(int) ) );
    connect( newnote, SIGNAL( sigKilled(QString) ),
             this,    SLOT( slotNoteKilled(QString) ) );

    m_NoteList.insert( thename, newnote );
    newnote->show();
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
    KNoteConfigDlg tmpconfig( m_defaults, i18n("KNotes Defaults") );
    tmpconfig.exec();
}

void KNotesApp::slotToNote( int id )
{
    //tell the WM to give this note focus
    QString name = m_note_menu->text( id );

    //if it's already showing, we need to change to it's desktop
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


void KNotesApp::mouseReleaseEvent( QMouseEvent * e)
{
    if ( rect().contains( e->pos() ) && e->button() == LeftButton ) {
	slotPrepareNoteMenu();
	m_note_menu->popup( e->globalPos() );
	return;
    }

    KSystemTray::mouseReleaseEvent( e );
}


#include "knotesapp.moc"
