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

#include "knote.h"
#include "knoteconfigdlg.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kwin.h>
#include <kmessagebox.h>
#include <kstddirs.h>
#include <kprocess.h>
#include <kdebug.h>
#include <klineeditdlg.h>
#include <kpopupmenu.h>
#include <kconfig.h>
#include <ksimpleconfig.h>

#include <qlabel.h>
#include <qpalette.h>
#include <qcolor.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qprinter.h>
#include <netwm.h>


//**** Initialization ************************************
KNote::KNote( QString configfile, bool oldconfig, QWidget* parent, const char* name )
    : QFrame( parent, name, WStyle_Customize | WStyle_NoBorderEx | WDestructiveClose ),
      m_configfile( configfile ),
      m_saveself( true )
{
    setFrameStyle( NoFrame );
    setMinimumSize( 20, 20 );

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
    m_menu->insertItem( i18n("Note Preferences..."), this, SLOT(slotPreferences(int)) );

    m_menu->insertSeparator();
    m_menu->insertItem( i18n("To Desktop"), m_desktop_menu );
    m_idAlwaysOnTop = m_menu->insertItem( i18n("Always On Top"), this, SLOT(slotAlwaysOnTop(int)));
// DISABLED for now because of a bug in the netwm classes
    m_menu->setItemEnabled( m_idAlwaysOnTop, false );

    m_menu->insertSeparator();
    m_menu->insertItem( i18n("Delete Note"), this, SLOT(slotKill(int)) );
    m_menu->insertItem( i18n("New Note"), this, SLOT(slotNewNote(int)) );

    //create the note header- button and label...
    m_button = new KNoteButton( this );
    m_button->setPixmap( BarIcon( "knotesclose" ) );
    connect( m_button, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

    m_label = new QLabel( this );
    m_label->setAlignment( AlignHCenter );
    m_label->installEventFilter( this );  //recieve events( for dragging & action menu )

    //create the note editor
    m_editor = new KNoteEdit( this );

    if( oldconfig ) {
        //read and convert the old configuration
        convertOldConfig();
    } else {
        //apply configuration settings...do before loading the text!
        slotApplyConfig();

        //load the saved text for this file...
        QDir notedir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
        QString datafile = "." + m_label->text() + "_data";
        if( notedir.exists( datafile ) )
        {
            //load the text file and put in m_editor...
            QString absfile = notedir.absFilePath( datafile );
            m_editor->readFile( absfile );
        }
    }
}

KNote::~KNote()
{
    QString name = m_label->text();
    emit sigKilled( name );

    if( m_saveself )
    {
        saveData();
        saveConfig();
    }
    else
    {
        QDir notedir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );

        if( !notedir.remove( name ) )
        {
            kdDebug() << "could not remove conf file" << endl;
        }
        if( !notedir.remove( "." + name + "_data" ) )
        {
            kdDebug() << "could not remove data file" << endl;
        }
    }

    delete m_editor;
    delete m_menu;
    delete m_desktop_menu;
    delete m_label;
    delete m_button;
}

//**** Private Utility functions *************************
void KNote::resizeEvent( QResizeEvent* qre )
{
    QFrame::resizeEvent( qre );

    int new_height = height();
    int new_width  = width();

    int headerHeight = m_label->sizeHint().height();

    m_button->setGeometry( new_width - headerHeight, 0, headerHeight, headerHeight );
    m_label->setGeometry( 0, 0, new_width - headerHeight, headerHeight );
    m_editor->setGeometry( 0, headerHeight, new_width, new_height - headerHeight );
}

void KNote::closeEvent( QCloseEvent* e )
{
    saveDisplayConfig();

    QWidget::closeEvent( e );
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

void KNote::convertOldConfig()
{
    QFile infile( m_configfile );
    if( infile.open( IO_ReadOnly ) )
    {
        QTextStream input( &infile );

        // get the name
        m_label->setText( input.readLine() );

        // get the geometry
        QString geo = input.readLine();

        int pos, data[13];
        int n = 0;

        while( (pos = geo.find('+')) != -1 )
        {
            if( n < 13 )
                data[n++] = geo.left(pos).toInt();
            geo.remove( 0, pos + 1 );
        }
        if( n < 13 )
            data[n++] = geo.toInt();

        int note_desktop = data[0];
        if( data[11] == 1 )
            note_desktop = NETWinInfo::OnAllDesktops;

        resize( data[3], data[4] );
        if( data[1] >= 0 && data[2] >= 0 )   // just to be sure...
            move( data[1], data[2] );

        if( data[12] & 2048 )
        {
// DISABLED for now because of a bug in the netwm classes
//          KWin::setState( winId(), NET::StaysOnTop | NET::SkipTaskbar );
//          m_menu->setItemChecked( m_idAlwaysOnTop, true );
        } else
            KWin::setState( winId(), NET::SkipTaskbar );

        // get the foreground color
        uint red = input.readLine().toUInt();
        uint green = input.readLine().toUInt();
        uint blue = input.readLine().toUInt();
        QColor bg = QColor( red, green, blue );

        // get the background color
        red = input.readLine().toUInt();
        green = input.readLine().toUInt();
        blue = input.readLine().toUInt();
        QColor fg = QColor( red, green, blue );

        QPalette newpalette = palette();
        newpalette.setColor( QColorGroup::Background, bg );
        newpalette.setColor( QColorGroup::Base,       bg );
        newpalette.setColor( QColorGroup::Foreground, fg );
        newpalette.setColor( QColorGroup::Text,       fg );
        setPalette( newpalette );

        // set darker values for the label and button...
        m_label->setBackgroundColor( bg.dark( 120 ) );
        m_button->setBackgroundColor( bg.dark( 120 ) );

        // get the font
        QString fontfamily = input.readLine();
        if( fontfamily.isEmpty() )
            fontfamily = QString( "helvetica" );
        uint size = input.readLine().toUInt();
        size = QMAX( size, 4 );
        uint weight = input.readLine().toUInt();
        bool italic = ( input.readLine().toUInt() == 1 );

        QFont font( fontfamily, size, weight, italic );
        setFont( font );

        // 3d frame? Not supported yet!
        input.readLine();

        // autoindent
        bool indent = ( input.readLine().toUInt() == 1 );
        m_editor->setAutoIndentMode( indent );
        m_editor->setDefaultTabStop( 4 );

        // hidden
        bool hidden = ( input.readLine().toUInt() == 1 );

        // show the note
        if( !hidden && !isVisible() )
        {
            // HACK HACK
            if( note_desktop != NETWinInfo::OnAllDesktops )
            {
                slotToDesktop( note_desktop );   //to avoid flicker, call this before show()
                show();
            } else {
                show();
                //if this is called before show(), it won't work for sticky notes!!!
                slotToDesktop( note_desktop );
            }
        }

        // get the text
        m_editor->setText( input.readLine() );  // workaround for bug in KMultiLineEdit
        while( !input.atEnd() )
            m_editor->insertLine( input.readLine() );

        infile.close();
        infile.remove();

        // write the new configuration
        saveData();
        saveConfig();
        saveDisplayConfig();

        KSimpleConfig config( m_configfile );
        config.setGroup( "General" );
        config.writeEntry( "version", 2 );

        config.setGroup( "Display" );
        config.writeEntry( "fgcolor", fg );
        config.writeEntry( "bgcolor", bg );

        config.setGroup( "Actions" );                  // use the new default for this group
        config.writeEntry( "mail", "kmail --msg %f" );
        config.writeEntry( "print", "a2ps -P %p -1 --center-title=%t --underlay=KDE %f" );

        config.setGroup( "Editor" );
        config.writeEntry( "autoindent", indent );
        config.writeEntry( "font", font );
        config.writeEntry( "tabsize", 4 );
        config.sync();
    } else
        kdDebug() << "could not open input file" << endl;
}


//**** Public Interface **********************************
void KNote::setText( const QString& text )
{
    m_editor->setText( text );
}

void KNote::saveData()
{
    QString datafile = m_label->text();
    QDir notedir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );

    if( datafile == QString::null )
    {
        kdDebug() << "note name was missing, not saving anything" << endl;
        return;
    }

    QString absdatafile = notedir.absFilePath( "." + datafile + "_data" );
    m_editor->dumpToFile( absdatafile );
}

void KNote::saveConfig()
{
    //all that needs to get saved here is the size and name
    //everything else would have been saved by the preferences dialog

    KSimpleConfig config( m_configfile );

    //store config settings...
    //need to save the new size to KSimpleConfig object
    config.setGroup( "Display" );
    config.writeEntry( "width", width() );
    config.writeEntry( "height", height() );

    //save name....
    config.setGroup( "Data" );
    if( m_label )
        config.writeEntry( "name", m_label->text() );
}

void KNote::saveDisplayConfig()
{
    KSimpleConfig config( m_configfile );
    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop | NET::WMState );

    config.setGroup( "WindowDisplay" );

    config.writeEntry( "desktop", wm_client.desktop() );
    config.writeEntry( "state", wm_client.state() );
    config.writeEntry( "position", pos() );
}

QString KNote::getName()
{
    if( m_label )
        return m_label->text();
    else
        return QString::null;
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

void KNote::slotApplyConfig()
{
    KSimpleConfig config( m_configfile );

    //do the Editor group: tabsize, autoindent, font, fontsize, fontstyle
    config.setGroup( "Editor" );

    QFont def( "helvetica" );
    def = config.readFontEntry( "font", &def ) ;
    setFont( def );

    uint tab_size = config.readUnsignedNumEntry( "tabsize", 4 );
    m_editor->setDefaultTabStop( tab_size );

    bool indent = true;
    indent = config.readBoolEntry( "autoindent", &indent );
    m_editor->setAutoIndentMode( indent );

    //do the Data Group- name, data
    config.setGroup( "Data" );

    QString notename = config.readEntry( "name", "KNotes" );
    m_label->setText( notename );


    //do Display group- width, height, bgcolor, fgcolor, transparent
    //do this after the editor part so that the label can adjust it's
    //size to the font on a resize event
    config.setGroup( "Display" );

    uint width  = config.readUnsignedNumEntry( "width", 200 );
    uint height = config.readUnsignedNumEntry( "height", 200 );
    resize( width, height );

    //create a pallete...
    QColor bg = config.readColorEntry( "bgcolor", &(Qt::yellow) );
    QColor fg = config.readColorEntry( "fgcolor", &(Qt::black) );
    QPalette newpalette = palette();
    newpalette.setColor( QColorGroup::Background, bg );
    newpalette.setColor( QColorGroup::Base,       bg );
    newpalette.setColor( QColorGroup::Foreground, fg );
    newpalette.setColor( QColorGroup::Text,       fg );
    setPalette( newpalette );

    //set darker values for the label and button...
    m_label->setBackgroundColor( bg.dark( 120 ) );
    m_button->setBackgroundColor( bg.dark( 120 ) );

    config.setGroup( "WindowDisplay" );
    int note_desktop = config.readNumEntry( "desktop", KWin::currentDesktop() );
    ulong note_state = config.readUnsignedLongNumEntry( "state", NET::SkipTaskbar );
    QPoint default_position = QPoint( -1, -1 );
    QPoint position  = config.readPointEntry( "position", &default_position );

    KWin::setState( winId(), note_state );
    if( note_state & NET::StaysOnTop )
    {
        m_menu->setItemChecked( m_idAlwaysOnTop, true );
    }
    if( position != default_position )
        move( position );                    //do before calling show() to avoid flicker
    if( note_desktop != 0 && !isVisible() )
    {
        // HACK HACK
        if( note_desktop != NETWinInfo::OnAllDesktops )
        {
            slotToDesktop( note_desktop );   //to avoid flicker, call this before show()
            show();
        } else {
            show();
            //if this is called before show(), it won't work for sticky notes!!!
            slotToDesktop( note_desktop );
        }
    }
}

void KNote::slotToDesktop( int id )
{
    if( id == 0 || id == NETWinInfo::OnAllDesktops )
        KWin::setOnAllDesktops( winId(), true );
    else
        KWin::setOnDesktop( winId(), id );
}

void KNote::slotAlwaysOnTop( int )
{
    if ( KWin::info(winId()).state & NET::StaysOnTop )
        KWin::clearState( winId(), NET::StaysOnTop );
    else
        KWin::setState( winId(), KWin::info(winId()).state | NET::StaysOnTop );

    m_menu->setItemChecked( m_idAlwaysOnTop, KWin::info(winId()).state & NET::StaysOnTop );
}

void KNote::slotPrepareDesktopMenu()
{
    NETRootInfo wm_root( qt_xdisplay(), NET::NumberOfDesktops | NET::DesktopNames );
    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop );

    QStringList desktops;
    int num = wm_root.numberOfDesktops();
    for( int i = 1; i <= num; i++ )
    {
        desktops.append( QString::fromUtf8(wm_root.desktopName( i )) );
    }

    //fill m_desktop_menu with the available desktop names
    bool alldesktops = false;
    int note_desktop = wm_client.desktop();

    if( note_desktop == NETWinInfo::OnAllDesktops )
        alldesktops = true;

    m_desktop_menu->clear();
    m_desktop_menu->insertItem( i18n( "&All desktops" ), 0 );
    if( alldesktops )
        m_desktop_menu->setItemChecked( 0, true );
    m_desktop_menu->insertSeparator( -1 );

    uint id;
    QStringList::Iterator it = desktops.begin();
    int num_desktops = (int)desktops.count();
    for( int i = 1; i <= num_desktops; i++, it++ )
    {
        id = m_desktop_menu->insertItem( QString("&%1 %2").arg(i).arg( *it ), i );
        if( note_desktop == i )
        {
            m_desktop_menu->setItemChecked( id, true );
        }
    }
}

void KNote::slotRename( int /*id*/ )
{
    QString newname;
    QString oldname = m_label->text();
    QString oldname_data = "." + oldname + "_data";
    QDir notedir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );

    while( true )
    {
        //pop up dialog to get the new name
        bool ok;
        newname = KLineEditDlg::getText( i18n("Please enter the new name"),
                                        oldname, &ok, this );
        if ( !ok ) // handle cancel
            return;

        //check the name to make sure that it's not already used
        if( notedir.exists( newname ) )
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
    saveData();
    saveConfig();
    saveDisplayConfig();

    if( notedir.rename( oldname, newname ) )
    {
        //open new config
        m_configfile = notedir.absFilePath( newname );
        KSimpleConfig config( m_configfile );

        //redo the label text
        m_label->setText( newname );
        config.setGroup( "Data" );
        config.writeEntry( "name", newname );

        //emit signal with oldname and newname
        emit sigRenamed( oldname, newname );

        if( notedir.exists( oldname_data ) )
        {
            notedir.rename( oldname_data, "."+newname+"_data" );
        }
    }
    else
    {
        kdDebug() << "rename failed" << endl;
    }
}

void KNote::slotInsDate( int /*id*/ )
{
    int line, column;
    m_editor->getCursorPosition( &line, &column );
    m_editor->insertAt( KGlobal::locale()->formatDateTime(QDateTime::currentDateTime()),
                        line, column );
}

void KNote::slotPreferences( int /*id*/ )
{
    saveConfig();

    //launch config dialog...
    KNoteConfigDlg configDlg( m_configfile, i18n("Local Settings") );
    connect( &configDlg, SIGNAL( updateConfig() ),
             this, SLOT( slotApplyConfig() ) );

    //launch preferences dialog...
    configDlg.show();
}

void KNote::slotClose()
{
//    saveData();
//    saveConfig();

    hide(); //just hide the note so it's still available from the dock window
}

void KNote::slotKill( int /*id*/ )
{
    m_saveself = false;
    close( true );
}

void KNote::slotMail( int /*id*/ )
{
    saveData();
    KSimpleConfig config( m_configfile, true );
    QDir notedir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );

    //sync up the data on note and the data file
    QString data_name = "." + m_label->text() + "_data";
    QString msg_body = notedir.absFilePath( data_name );

    //get the mail action command
    config.setGroup( "Actions" );
    QString mail_cmd = config.readEntry( "mail", "kmail --msg %f" );
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
    saveData();
    KSimpleConfig config( m_configfile, true );
    QDir notedir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
    QPrinter printer;

    if( printer.setup() )
    {
        config.setGroup( "Actions" );
        QString printstr = config.readEntry(
                              "print",
                              "a2ps -P %p -1 --center-title=%t --underlay=KDE %f"
                           );

        QString printername = printer.printerName();
        QString title = m_label->text();
        QString datafile = notedir.absFilePath( "." + title + "_data" );

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

void KNote::slotNewNote( int id )
{
    emit sigNewNote( id );
}

#include "knote.moc"
#include "knotebutton.moc"
