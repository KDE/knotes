

#include "knotesapp.h"
#include "knoteconfigdlg.h"

#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>


KNotesApp::	KNotesApp()
{
	//make sure I copy over the knotesrc to a local/writeable file- some
	QString globalConfigFile = KGlobal::dirs()->findResource( "config", "knotesrc" );
    QString str_confdir = KGlobal::dirs()->saveLocation( "config" );
    kdDebug() << "can save config at: " << str_confdir << endl;
	QDir confdir( str_confdir );
	
	if( !confdir.exists( "knotesrc" ) )
	{
		//copy over the default config file...avoid some problems with session management
		QFile gconfigfile( globalConfigFile );
	    gconfigfile.open( IO_ReadOnly );
    	
    	QFile nconfigfile( str_confdir + "knotesrc" );
    	nconfigfile.open( IO_WriteOnly );

    	QTextStream input( &gconfigfile );
    	QTextStream output( &nconfigfile );

    	for( QString curr = input.readLine(); curr != QString::null; curr = input.readLine() )
			output << curr << endl;

    	gconfigfile.close();
    	nconfigfile.close();
    	kdDebug() << "saved the default config file in KDEHOME" << endl;
	} else kdDebug() << "default config already exists" << endl;

	//create the dock widget....
	setPixmap( KGlobal::iconLoader()->loadIcon( "knotes", KIcon::Desktop ) );
	KPopupMenu* menu = contextMenu();
	menu->insertItem( i18n("New Note"), this, SLOT(slotNewNote(int)) );
	menu->insertItem( i18n("Preferences..."), this, SLOT(slotPreferences(int)) );		
	
 	//initialize saved notes, if none create a note...
	QString str_notedir = KGlobal::dirs()->saveLocation( "appdata", "notes/" );
 	QDir notedir( str_notedir );
 	QStringList notes = notedir.entryList( "*" );

 	int count = 0;
 	for( QStringList::Iterator i = notes.begin(); i != notes.end(); ++i )
 	{
 		kdDebug() << "checking file: " << *i << endl;
		if( *i != "." && *i != ".." ) //ignore these
 		{
 			QString configfile = notedir.absFilePath( *i );
 			kdDebug() << "restoring note, file: " << configfile << endl;
   			KConfig* tmp = new KConfig( configfile );
   			KNote* tmpnote = new KNote( tmp );
   			m_NoteList[*i] = tmpnote;
   			tmpnote->show();
			++count;
		}
	}
	
	if( count == 0 )
		slotNewNote();
}


KNotesApp::~KNotesApp()
{
}

void KNotesApp::slotNewNote( int /*id*/ )
{
	QString globalConfigFile = KGlobal::dirs()->findResource( "config", "knotesrc" );
	QString datadir = KGlobal::dirs()->saveLocation( "appdata", "notes/" );
	kdDebug() << "KNotesApp::slotNewNote, using template: " << globalConfigFile << endl;
	
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

    //copy the default config file to $KDEHOME/share/apps/knotes2/notes/filename
    QFile gconfigfile( globalConfigFile );
    gconfigfile.open( IO_ReadOnly );

    QFile nconfigfile( datadir + thename );
    nconfigfile.open( IO_WriteOnly );

    QTextStream input( &gconfigfile );
    QTextStream output( &nconfigfile );

    for( QString curr = input.readLine(); curr != QString::null; curr = input.readLine() )
		output << curr << endl;

    gconfigfile.close();
    nconfigfile.close();


    //then create a new KConfig object for the new note, so it can save it's own data
    KConfig* newconfig = new KConfig( datadir + thename );
    newconfig->setGroup( "Data" );
    newconfig->writeEntry("name", thename );
    newconfig->sync();

	KNote* newnote = new KNote( newconfig );
	
	connect( newnote, SIGNAL( sigRenamed(QString&, QString&) ),
	         this,    SLOT( slotNoteRenamed(QString&, QString&) ) );
	connect( newnote, SIGNAL( sigNewNote(int) ),
	         this,    SLOT( slotNewNote(int) ) );	
	connect( newnote, SIGNAL( sigClosed(QString&) ),
	         this,    SLOT( slotNoteClosed(QString&) ) );
	
	m_NoteList[thename] = newnote;
	newnote->show();	
}

void KNotesApp::slotNoteRenamed( QString& oldname, QString& newname )
{
	KNote* tmp = m_NoteList[oldname];
	m_NoteList[newname] = tmp;
	m_NoteList.remove( oldname );
}

void KNotesApp::slotNoteClosed( QString& name )
{
	kdDebug() << "removed note: " << name << endl;
	m_NoteList.remove( name );
}

void KNotesApp::slotPreferences( int /*id*/ )
{
	QString globalConfigFile = KGlobal::dirs()->findResource( "config", "knotesrc" );
	kdDebug() << "globalConfigFile = " << globalConfigFile << endl;
	KConfig GlobalConfig( globalConfigFile );
	
	//launch preferences dialog...
	KNoteConfigDlg tmpconfig( &GlobalConfig, i18n("KNotes Defaults") );
	tmpconfig.exec();
	kdDebug() << "after global settings configured" << endl;
}
#include "knotesapp.moc"
