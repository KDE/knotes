

#include "knote.h"
#include "knoteconfigdlg.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kwin.h>
#include <kmessagebox.h>
#include <klineeditdlg.h>
#include <kstddirs.h>
#include <kprocess.h>
#include <kdebug.h>

#include <qpalette.h>
#include <qcolor.h>
#include <qdir.h>
#include <iostream.h>

//**** Initialization ************************************
KNote::KNote( KConfig* config, QWidget* parent, const char* name )
        : QFrame( parent, name , WStyle_Customize | WStyle_NoBorderEx | WDestructiveClose )
{
        kdDebug() << "start Knote::Knote" << endl;

        m_config = config;
        m_notedir = new QDir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
        setFrameStyle( NoFrame );

        //create the note header- button and label...
        m_button = new KNoteButton( this );
        m_button->setPixmap( BarIcon( "knotesclose" ) );
        connect( m_button, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

        m_label = new QLabel( this );
        m_label->setAlignment( AlignHCenter );
        m_label->setText( m_config->entryMap("Data")["name"] );
        m_label->installEventFilter( this );  //recieve events( for dragging & action menu )

        m_editor = new KNoteEdit( this );

        //load the saved text for this file...
        QString datafile = "." + m_config->entryMap("Data")["name"] + "_data";
        if( m_notedir->exists( datafile ) )
        {
                //load the text file and put in m_editor...
                QString absfile = m_notedir->absFilePath( datafile );
                m_editor->readFile( absfile );
        }

    //apply configuration settings
    slotApplyConfig();

    //create the menu items for note- not the editor...
        //rename, mail, print, insert date, close, delete
        m_menu = new KPopupMenu( this );
        m_menu->insertItem( i18n("Rename"),      this, SLOT(slotRename(int)) );
        m_menu->insertItem( i18n("Mail"),        this, SLOT(slotMail(int)) );
        m_menu->insertItem( i18n("Print"),       this, SLOT(slotPrint(int)) );
        m_menu->insertItem( i18n("Insert Date"), this, SLOT(slotInsDate(int)) );
        m_menu->insertSeparator();
        m_menu->insertItem( i18n("Note Preferences..."), this, SLOT(slotConfig(int)) );
        m_menu->insertSeparator();
        m_menu->insertItem( i18n("Delete Note"), this, SLOT(slotKill(int)) );
}


KNote::~KNote()
{
        //store data from multiline editor in the KConfig file...
        if( m_editor && m_config ) //m_config has been deleted when note is killed/not closed
        {
                QString datafile = m_config->entryMap("Data")["name"];
                if( datafile == QString::null )
                {
                        kdDebug() << "note name was missing, not saving anything" << endl;
                        return;
                }
                datafile = "." + datafile + "_data";
                kdDebug() << "saving data to: " << datafile << endl;
                QString absdatafile = m_notedir->absFilePath( datafile );
                m_editor->dumpToFile( absdatafile );
        }

        if( m_config )
                m_config->sync();
}


//**** Private Utility functions *************************
void KNote::resizeEvent( QResizeEvent* qre )
{
        QFrame::resizeEvent( qre );

        m_button->setGeometry( width() - 15, 0, 15, 15 );
        m_label->setGeometry( 0, 0, width() - 15, 15 );
        m_editor->setGeometry( 0, 15, width(), height() - 15 );
}

void KNote::slotApplyConfig()
{
        //do Display group- width, height, bgcolor, fgcolor, transparent
        int width  = (m_config->entryMap( "Display" )["width"]).toInt();
        int height = (m_config->entryMap( "Display" )["height"]).toInt();
        resize( width, height );

        //create a pallete...
        QColor bg = KNoteConfigDlg::getBGColor( *m_config );
        QColor fg = KNoteConfigDlg::getFGColor( *m_config );

        QPalette newpalette = palette();
        newpalette.setColor( QColorGroup::Background, bg );
        newpalette.setColor( QColorGroup::Base,       bg );
        newpalette.setColor( QColorGroup::Foreground, fg );
        newpalette.setColor( QColorGroup::Text,       fg );
        setPalette( newpalette );

        //set darker values for the label and button...
        m_label->setBackgroundColor( bg.dark( 120 ) );
        m_button->setBackgroundColor( bg.dark( 120 ) );

        //do the Editor group: tabsize, autoindent, font, fontsize, fontstyle
        QString str_font = m_config->entryMap("Editor")["font"];
        int font_size    =(m_config->entryMap("Editor")["fontsize"]).toInt();
        int fontstyle    =(m_config->entryMap("Editor")["fontstyle"]).toInt();
        m_editor->setFont( QFont( str_font, font_size ) );

        int tab_size = (m_config->entryMap("Editor")["tabsize"]).toInt();
        m_editor->setDefaultTabStop( tab_size );

        QString indent = m_config->entryMap("Editor")["autoindent"];
        if( indent == "yes" )
                 m_editor->setAutoIndentMode( true );
        else m_editor->setAutoIndentMode( false );

        //do Actions Group, mail, print, date

        //do the Data Group- name, data
        QString notename = m_config->entryMap("Data")["name"];
        m_label->setText( notename );
}


//**** Public Interface **********************************
void KNote::save()
{
}


//**** SLOTS *********************************************
bool KNote::eventFilter( QObject* o, QEvent* ev )
{
        QMouseEvent* e = (QMouseEvent*)ev;

        if( o == m_label )
        {
                if( ev->type() == QEvent::MouseButtonRelease )
                {
                        if( e->button() == LeftButton )
                        {
                                m_dragging = false;
                                m_label->releaseMouse();
                                raise();
                        }
                        if (e->button() == MidButton)
                                lower();
                        return true;
                }

                if( ev->type() == QEvent::MouseButtonPress && e->button() == LeftButton )
                {
                        m_pointerOffset = e->pos();
                        m_label->grabMouse(sizeAllCursor);
                        return true;
                }
                if( ev->type() == QEvent::MouseMove && m_label == mouseGrabber())
                {
                        if ( m_dragging )
                        {
                                move( QCursor::pos() - m_pointerOffset );
                        }
                        else
                        {
                                m_dragging = (
                                        (e->pos().x() - m_pointerOffset.x())
                                        *
                                        (e->pos().x() - m_pointerOffset.x())
                                        +
                                        (e->pos().y() - m_pointerOffset.y())
                                        *
                                        (e->pos().y() - m_pointerOffset.y())
                                        >= 9 );
                        }
                        return true;
                }

                if( m_menu && ( ev->type() == QEvent::MouseButtonPress )
                           && ( e->button() == RightButton ) )
                {
                        m_menu->popup( QCursor::pos() );
                        return true;
                }

                return m_label->eventFilter( o, ev );
        }

        return QWidget::eventFilter( o, ev );
}

void KNote::slotMail( int /*id*/ )
{
        //sync up the data on note and the data file
        QString data_name = m_config->entryMap("Data")["name"] + "_data";
        data_name = m_notedir->absFilePath( data_name );
        m_editor->dumpToFile( data_name );

        QString fname = "\"";
        fname += data_name;
        fname += "\"";

        //KMail doesn't really respect the --msg option it seems??
        KProcess mail;
        mail << "kmail" << "--composer" << "--msg" << fname;
        if( !mail.start( KProcess::DontCare ) )
                cerr << "could not start process" << endl;
}


void KNote::slotRename( int /*id*/ )
{
        QString newname;
        QString oldname = m_config->entryMap("Data")["name"];
        QString oldname_data = oldname + "_data";

        while( true )
        {
            //pop up dialog to get the new name
            bool ok;
            newname = KLineEditDlg::getText( i18n("Please enter the new name"),
                                             QString::null, &ok, this );
            if ( !ok ) // handle cancel
                return;

            //check the name to make sure that it's not already used
            if( m_notedir->exists( newname ) )
            {
                KMessageBox::sorry( this,
                                    i18n("There is already a note with that name") );
            }
            else
                if( newname.isEmpty() )
                    KMessageBox::sorry( this,
                                        i18n("A name must have at least one character") );
                else break;
        }

        //close config, copy old file to new name
        m_config->setGroup( "Data" );
        m_config->writeEntry( "name", newname );
        m_config->sync();

        delete m_config;
        if( !m_notedir->rename( oldname, newname ) )
                kdDebug() << "rename failed" << endl;
        if( !m_notedir->rename( "."+oldname+"_data", "."+newname+"_data" ) )
                kdDebug() << "rename of data file failed" << endl;

        //open new config
        QString newconfig = m_notedir->absFilePath( newname );
        m_config = new KConfig( newconfig );

        //redo the label text
        m_label->setText( newname );

        //emit signal with oldname and newname
        emit sigRenamed( oldname, newname );
}

void KNote::slotInsDate( int /*id*/ )
{
        int line, column;
        m_editor->getCursorPosition( &line, &column );
        m_editor->insertAt( KGlobal::locale()->formatDateTime(QDateTime::currentDateTime()),
                            line, column );
}

void KNote::slotConfig ( int /*id*/ )
{
        //launch config dialog...
        KNoteConfigDlg* localConfig = new KNoteConfigDlg( m_config, i18n("Local Settings") );
        connect( localConfig, SIGNAL( updateConfig() ),
                 this, SLOT( slotApplyConfig() ) );

        //launch preferences dialog...
        localConfig->show();
}

void KNote::slotClose()
{
        //save the note text...
        QString datafile = "." + m_config->entryMap("Data")["name"] + "_data";
        QString absname = m_notedir->absFilePath( datafile );
        m_editor->dumpToFile( absname );

        m_config->sync();

        close();

        emit sigClosed( m_config->entryMap("Data")["name"] );
}

void KNote::slotKill( int /*id*/ )
{
    //delete the config file...
        QString conf_name = m_config->entryMap("Data")["name"];
    m_notedir->remove( conf_name );
    m_notedir->remove( "." + conf_name + "_data" );

    kdDebug() << "KNote::slotKill, removed file: " << conf_name << endl;
    kdDebug() << "KNote::slotKill, removed file: " << "." + conf_name + "_data" << endl;

        //don't save anything- m_config checked in destructor
        emit sigClosed( m_config->entryMap("Data")["name"] );
        delete m_config;
        m_config = NULL;

        close();
 }


void KNote::slotPrint( int /*id*/ )
{
}
#include "knote.moc"
