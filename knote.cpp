

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
#include <qprinter.h>
#include <iostream.h>

#include <netwm.h>

//**** Initialization ************************************
KNote::KNote( KSimpleConfig* config, QWidget* parent, const char* name )
    : QFrame( parent, name , WStyle_Customize | WStyle_NoBorderEx | WDestructiveClose ),
      m_config( config )
{
    m_notedir = new QDir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
    setFrameStyle( NoFrame );
    setMinimumSize( 100, 100 );

    //create the note header- button and label...
    m_button = new KNoteButton( this );
    m_button->setPixmap( BarIcon( "knotesclose" ) );
    connect( m_button, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

    m_label = new QLabel( this );
    m_label->setAlignment( AlignHCenter );
    m_config->setGroup( "Data" );
    m_label->setText( m_config->readEntry( "name" ) );
    m_label->installEventFilter( this );  //recieve events( for dragging & action menu )
    m_headerHeight = m_label->sizeHint().height();

    m_editor = new KNoteEdit( this );

    //load the saved text for this file...
    QString datafile = "." + m_label->text() + "_data";
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
    m_desktop_menu = new KPopupMenu( this );
    connect( m_desktop_menu, SIGNAL( aboutToShow() ),
             this,           SLOT( slotPrepareDesktopMenu() ) );
    connect( m_desktop_menu, SIGNAL( activated(int) ),
             this,           SLOT(slotToDesktop(int) ) );

    m_menu = new KPopupMenu( this );
    m_menu->insertItem( i18n("Rename"), this, SLOT(slotRename(int)) );
    m_menu->insertItem( i18n("Mail"), this, SLOT(slotMail(int)) );
    m_menu->insertItem( i18n("Print"), this, SLOT(slotPrint(int)) );
    m_menu->insertItem( i18n("Insert Date"), this, SLOT(slotInsDate(int)) );
    m_menu->insertItem( i18n("Note Preferences..."), this, SLOT(slotConfig(int)) );

    m_menu->insertSeparator();
    m_menu->insertItem( i18n("To Desktop"), m_desktop_menu );
    m_idAlwaysOnTop = m_menu->insertItem( i18n("Always On Top"), this, SLOT(slotAlwaysOnTop(int)));

    m_menu->insertSeparator();
    m_menu->insertItem( i18n("Delete Note"), this, SLOT(slotKill(int)) );

    NETWinInfo info( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMState );
    unsigned long state = info.state();
    kdDebug() << "my state is: " << state << endl;
    info.setState( NET::SkipTaskbar, NET::SkipTaskbar );
    state = info.state();
    kdDebug() << "new state is: " << state << endl;
}


KNote::~KNote()
{
    //store data from multiline editor in the KConfig file...
    if( m_config )
    {
        save();
        m_config->sync();
    }
}


//**** Private Utility functions *************************
void KNote::resizeEvent( QResizeEvent* qre )
{
    QFrame::resizeEvent( qre );

    int new_height = height();
    int new_width  = width();

    m_button->setGeometry( new_width - m_headerHeight, 0, m_headerHeight, m_headerHeight );
    m_label->setGeometry( 0, 0, new_width - m_headerHeight, m_headerHeight );
    m_editor->setGeometry( 0, m_headerHeight, new_width, new_height - m_headerHeight );

    //need to save the new size to KConfig object
    m_config->setGroup( "Display" );
    m_config->writeEntry( "width", new_width );
    m_config->writeEntry( "height", new_height );
}

void KNote::slotApplyConfig()
{
    //do Display group- width, height, bgcolor, fgcolor, transparent
    m_config->setGroup( "Display" );

    uint width = m_config->readUnsignedNumEntry( "width", 200 );
    uint height = m_config->readUnsignedNumEntry( "height", 200 );
    resize( width, height );

    //create a pallete...
    QColor bg = m_config->readColorEntry( "bgcolor", &(Qt::yellow) );
    QColor fg = m_config->readColorEntry( "fgcolor", &(Qt::black) );
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
    m_config->setGroup( "Editor" );

    QFont def( "helvetica" );
    def = m_config->readFontEntry( "font", &def ) ;
    m_editor->setFont( def );

    uint tab_size = m_config->readUnsignedNumEntry( "tabsize", 4 );
    m_editor->setDefaultTabStop( tab_size );

    bool indent = true;
    indent = m_config->readBoolEntry( "autoindent", &indent );
    m_editor->setAutoIndentMode( indent );

    //do the Data Group- name, data
    m_config->setGroup( "Data" );

    QString notename = m_config->readEntry( "name", "KNotes" );
    m_label->setText( notename );

}


//**** Public Interface **********************************
void KNote::save()
{
    QString datafile = m_config->entryMap("Data")["name"];
    if( datafile == QString::null )
    {
        kdDebug() << "note name was missing, not saving anything" << endl;
        return;
    }
    QString absdatafile = m_notedir->absFilePath( "." + datafile + "_data" );
    m_editor->dumpToFile( absdatafile );
}

void KNote::setOnDesktop( uint id )
{
    NETWinInfo info( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop );
    if( id == 0 )
    {
        info.setDesktop( NETWinInfo::OnAllDesktops );
    }
    else
    {
        info.setDesktop( id );
    }
}

uint KNote::currDesktop()
{
    NETWinInfo info( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop );
    return info.desktop();
}

void KNote::slotToDesktop( int id )
{
    setOnDesktop( id );
}

void KNote::slotAlwaysOnTop( int )
{
    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMState );
    unsigned long currstate = wm_client.state();
    kdDebug() << "currstate = " << currstate << endl;
    if( (currstate & NET::StaysOnTop) == NET::StaysOnTop )
    {
        wm_client.setState( 0, NET::StaysOnTop );
        m_menu->setItemChecked( m_idAlwaysOnTop, false );
    }
    else
    {
        wm_client.setState( NET::StaysOnTop, NET::StaysOnTop );
        m_menu->setItemChecked( m_idAlwaysOnTop, true );
    }
    currstate = wm_client.state();
    kdDebug() << "newstate = " << currstate << endl;
}

QString KNote::getName()
{
    if( m_label )
        return m_label->text();
    else return QString::null;
}


//**** SLOTS *********************************************
void KNote::setFocus()
{
    m_editor->setFocus();
}

void KNote::show()
{
    QFrame::show();

}

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

void KNote::slotPrepareDesktopMenu()
{
    NETRootInfo wm_root( qt_xdisplay(), NET::NumberOfDesktops );
    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop );

    QStringList desktops;
    int num = wm_root.numberOfDesktops();
    for( int i = 1; i <= num; i++ )
    {
        desktops.append( wm_root.desktopName( i ) );
    }

    //fill m_desktop_menu with the available desktop names
    bool alldesktops = false;
    if( wm_client.desktop() == NETWinInfo::OnAllDesktops )
        alldesktops = true;

    m_desktop_menu->clear();
    m_desktop_menu->insertItem( i18n( "&All desktops" ), 0 );
    if( alldesktops )
        m_desktop_menu->setItemChecked( 0, true );
    m_desktop_menu->insertSeparator( -1 );

    uint id;
    QStringList::Iterator it = desktops.begin();
    for( uint i = 1; i <= desktops.count(); i++, it++ )
    {
        id = m_desktop_menu->insertItem( QString("&%1 %2").arg(i).arg( *it ), i );
        if( ( wm_client.desktop() == i ) && !alldesktops )
        {
            m_desktop_menu->setItemChecked( id, true );
        }
    }
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
    m_config = new KSimpleConfig( newconfig );

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
    hide(); //just hide the note so it's still available from the dock window
    save();
}

void KNote::slotKill( int /*id*/ )
{
    //delete the config file...
    m_config->setGroup( "Data" );
    QString conf_name = m_config->readEntry( "name" );
    delete m_config;
    m_config = NULL;

    m_notedir->remove( conf_name );
    m_notedir->remove( "." + conf_name + "_data" );

    emit sigKilled( conf_name );
    close();
}

void KNote::slotMail( int /*id*/ )
{
    save();

    //sync up the data on note and the data file
    QString data_name = "." + m_label->text() + "_data";
    QString msg_body = m_notedir->absFilePath( data_name );

    //get the mail action command
    m_config->setGroup( "Actions" );
    QString mail_cmd = m_config->readEntry( "mail", "kmail --msg %f" );
    QStringList cmd_list = QStringList::split( QChar(' '), mail_cmd );

    KProcess mail;
    for( QStringList::Iterator it = cmd_list.begin();
    it != cmd_list.end();
            ++it )
    {
        if( *it == "%f" )
            mail << msg_body;
        else
            mail << *it;
    }

    if( !mail.start( KProcess::DontCare ) )
        kdDebug() << "could not start process" << endl;
}

void KNote::slotPrint( int /*id*/ )
{
    save();

    QPrinter printer;
    if( printer.setup() )
    {
        m_config->setGroup( "Actions" );
        QString printstr = m_config->readEntry(
                              "print",
                              "a2ps -P %p -1 --center-title=%t --underlay=KDE %f"
                           );

        QString printername = printer.printerName();
        QString title = m_label->text();
        QString datafile = m_notedir->absFilePath( "." + title + "_data" );

        QStringList cmd_list = QStringList::split( QChar(' '), printstr );
        KProcess printjob;
        for( QStringList::Iterator it = cmd_list.begin();
             it != cmd_list.end(); ++it )
        {
            if( *it == "%p" )
                printjob << printername;
            else if( *it == "%t" )
                printjob << title;
            else if( *it == "%f" )
                printjob << datafile;
            else
                printjob << *it;
        }

        bool result = printjob.start( KProcess::Block, KProcess::NoCommunication );
        if( !result )
            kdDebug() << "printing failed" << endl;
    }
}

#include "knote.moc"
