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

#include "knote.h"
#include "knotebutton.h"
#include "knoteedit.h"
#include "knoteconfigdlg.h"

#include <qlabel.h>
#include <qpalette.h>
#include <qcolor.h>
#include <qfile.h>
#include <qtextstream.h>

#include <kprinter.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <klineeditdlg.h>
#include <kpopupmenu.h>
#include <kmdcodec.h>
#include <kio/netaccess.h>
#include <kwin.h>
#include <netwm.h>

// fscking X headers
#ifdef FocusIn
#undef FocusIn
#endif
#ifdef FocusOut
#undef FocusOut
#endif

// -------------------- Initialisation -------------------- //
KNote::KNote( const QString& file, bool load, QWidget* parent, const char* name )
  : QFrame( parent, name, WStyle_Customize | WStyle_NoBorderEx | WDestructiveClose ),
      m_noteDir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) ),
      m_configFile( file )
{
    // create the menu items for the note - not the editor...
    // rename, mail, print, insert date, close, delete, new note
    m_desktop_menu = new KPopupMenu( this );
    connect( m_desktop_menu, SIGNAL( aboutToShow() ),
             this,           SLOT( slotPrepareDesktopMenu() ) );
    connect( m_desktop_menu, SIGNAL( activated(int) ),
             this,           SLOT( slotToDesktop(int) ) );

    m_menu = new KPopupMenu( this );
    m_menu->insertItem( i18n("Rename"), this, SLOT(slotRename()) );
    m_menu->insertItem( i18n("Mail"), this, SLOT(slotMail()) );
    m_menu->insertItem( i18n("Print"), this, SLOT(slotPrint()) );
    m_menu->insertItem( i18n("Insert Date"), this, SLOT(slotInsDate()) );
    m_menu->insertItem( i18n("Note Preferences..."), this, SLOT(slotPreferences()) );

    m_menu->insertSeparator();
    m_menu->insertItem( i18n("New Note"), this, SLOT(slotNewNote()) );
    m_menu->insertItem( i18n("Delete Note"), this, SLOT(slotKill()) );

    m_menu->insertSeparator();
    m_menu->insertItem( i18n("To Desktop"), m_desktop_menu );
    m_idAlwaysOnTop = m_menu->insertItem( i18n("Always On Top"), this, SLOT(slotAlwaysOnTop()));
    m_menu->setItemEnabled( m_idAlwaysOnTop, true );

    // create the note header, button and label...
    m_button = new KNoteButton( this );
    m_button->setPixmap( BarIcon( "knotesclose" ) );
    connect( m_button, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

    m_label = new QLabel( this );
    m_label->setAlignment( AlignHCenter );
    m_label->installEventFilter( this );  // recieve events (for dragging & action menu)

    // create the note editor
    m_editor = new KNoteEdit( this );
    m_editor->installEventFilter( this ); // recieve events (for modified)

    setFocusProxy( m_editor );

    // set up the look&feel of the note
    setFrameStyle( WinPanel | Raised );
    setLineWidth( 1 );
    setMinimumSize( 20, 20 );
    setMargin( 5 );

    // now create or load the data and configuration
    bool oldconfig = false;
    // WARNING: if the config file doesn't exist a new note will be created!
    if ( load && m_noteDir.exists( m_configFile ) )
    {
        KSimpleConfig* test = new KSimpleConfig( m_noteDir.absFilePath( m_configFile ), true );
        test->setGroup( "General" );
        oldconfig = ( test->readNumEntry( "version", 1 ) == 1 );
        delete test;
    }
    else
    {
        m_label->setText( m_configFile );

        // set the new configfile's name...
        for ( int i = 1; ; i++ )
        {
            m_configFile = QString( "KNote %1" ).arg(i);
            if ( !m_noteDir.exists( m_configFile ) )
                break;
        }

        // ...and "fill" it with the default config
        KIO::NetAccess::copy( KURL( KGlobal::dirs()->findResource( "config", "knotesrc" ) ),
                              KURL( m_noteDir.absFilePath( m_configFile ) ) );
    }

    if ( oldconfig ) {
        //read and convert the old configuration
        convertOldConfig();
    } else {
        // load the display configuration of the note
        KSimpleConfig config( m_noteDir.absFilePath( m_configFile ) );
        config.setGroup( "Display" );
        uint width  = config.readUnsignedNumEntry( "width", 200 );
        uint height = config.readUnsignedNumEntry( "height", 200 );
        resize( width, height );

        config.setGroup( "WindowDisplay" );
        int note_desktop = config.readNumEntry( "desktop", KWin::currentDesktop() );
        ulong note_state = config.readUnsignedLongNumEntry( "state", NET::SkipTaskbar );
        QPoint default_position = QPoint( -1, -1 );
        QPoint position  = config.readPointEntry( "position", &default_position );

        KWin::setState( winId(), note_state );
        if ( note_state & NET::StaysOnTop )
            m_menu->setItemChecked( m_idAlwaysOnTop, true );

        if ( position != default_position )
            move( position );                    // do before calling show() to avoid flicker

        // read configuration settings...
        slotApplyConfig();

        // show the note if desired
        if ( note_desktop != 0 && !isVisible() )
        {
            // HACK HACK
            if( note_desktop != NETWinInfo::OnAllDesktops )
            {
                // to avoid flicker, call this before show()
                slotToDesktop( note_desktop );
                show();
            } else {
                show();
                // if this is called before show(),
                // it won't work for sticky notes!!!
                slotToDesktop( note_desktop );
            }
        }

        // load the saved text and put it in m_editor...
        QString datafile = "." + m_configFile + "_data";
        if ( m_noteDir.exists( datafile ) )
        {
            QString absfile = m_noteDir.absFilePath( datafile );
            m_editor->readFile( absfile );
        }

        // TODO: remove this HACK!
        slotApplyConfig();
    }
}

KNote::~KNote()
{
    emit sigKilled( m_label->text() );

    delete m_editor;
    delete m_menu;
    delete m_desktop_menu;
    delete m_label;
    delete m_button;
}


// -------------------- public member functions -------------------- //

void KNote::saveData() const
{
    QString datafile = m_noteDir.absFilePath( "." + m_configFile + "_data" );
    m_editor->dumpToFile( datafile );
    m_editor->setModified( false );
}

void KNote::saveConfig() const
{
    //all that needs to get saved here is the size and name
    //everything else would have been saved by the preferences dialog
    KSimpleConfig config( m_noteDir.absFilePath( m_configFile ) );

    //store config settings...
    //need to save the new size to KSimpleConfig object
    config.setGroup( "Display" );
    config.writeEntry( "width", width() );
    config.writeEntry( "height", height() );

    //save name....
    config.setGroup( "Data" );
    config.writeEntry( "name", m_label->text() );
}

void KNote::saveDisplayConfig() const
{
    KSimpleConfig config( m_noteDir.absFilePath( m_configFile ) );
    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop | NET::WMState );

    config.setGroup( "WindowDisplay" );
    config.writeEntry( "desktop", wm_client.desktop() );
    config.writeEntry( "state", wm_client.state() );
    config.writeEntry( "position", pos() );
}

int KNote::noteId() const
{
    return m_configFile.mid( 6 ).toInt();
}

QString KNote::name() const
{
    return m_label->text();
}

QString KNote::text() const
{
    return m_editor->text();
}

void KNote::setName( const QString& name )
{
    m_label->setText( name );

    saveConfig();
}

void KNote::setText( const QString& text )
{
    m_editor->setText( text );
}

void KNote::sync( const QString& app )
{
    QByteArray sep( 1 );
    sep[0] = '\0';

    KMD5 hash;
    HASHHEX result;

    hash.update( m_label->text().utf8() );
    hash.update( sep );
    hash.update( m_editor->text().utf8() );
    hash.finalize();
    hash.hexDigest( result );

    if ( !hash.hasErrored() )
    {
        KSimpleConfig config( m_noteDir.absFilePath( m_configFile ) );

        config.setGroup( "Synchronisation" );
        config.writeEntry( app, result );
    }
    else
        kdWarning() << "Couldn't calculate digest because of an error!" << endl;

    hash.reset();
}

bool KNote::isNew( const QString& app ) const
{
    KSimpleConfig config( m_noteDir.absFilePath( m_configFile ) );

    config.setGroup( "Synchronisation" );
    QString hash = config.readEntry( app );
    return hash.isEmpty();
}

bool KNote::isModified( const QString& app ) const
{
    QByteArray sep( 1 );
    sep[0] = '\0';

    KMD5 hash;
    hash.update( m_label->text().utf8() );
    hash.update( sep );
    hash.update( m_editor->text().utf8() );
    hash.finalize();
    hash.hexDigest();

    KSimpleConfig config( m_noteDir.absFilePath( m_configFile ) );
    config.setGroup( "Synchronisation" );
    QString orig = config.readEntry( app );

    if ( hash.verify( orig.utf8() ) )   // returns false on error!
        return false;
    else
        return true;
}


// -------------------- public slots -------------------- //

void KNote::slotNewNote()
{
    emit sigNewNote();
}

void KNote::slotRename()
{
    //pop up dialog to get the new name
    bool ok;
    QString newname = KLineEditDlg::getText( i18n("Please enter the new name"),
                                             m_label->text(), &ok, this );
    if ( !ok ) // handle cancel
        return;

    if ( newname.isEmpty() ) {
        KMessageBox::sorry( this, i18n("A name must have at least one character") );
        return;
    }

    emit sigRenamed( m_label->text(), newname );
}

void KNote::slotClose()
{
    m_editor->clearFocus();
    hide(); //just hide the note so it's still available from the dock window
}

void KNote::slotKill()
{
    if ( !m_noteDir.remove( m_configFile ) )
        kdWarning() << "could not remove conf file for note " << m_label->text() << endl;

    if ( !m_noteDir.remove( "." + m_configFile + "_data" ) )
        kdWarning() << "could not remove data file for note " << m_label->text() << endl;

    delete this;
}

void KNote::slotInsDate()
{
    m_editor->insert( KGlobal::locale()->formatDateTime(QDateTime::currentDateTime()) );
}

void KNote::slotPreferences()
{
    saveConfig();

    //launch preferences dialog...
    KNoteConfigDlg configDlg( m_noteDir.absFilePath( m_configFile ),
                              i18n("Local Settings"), false );
    connect( &configDlg, SIGNAL( updateConfig() ), this, SLOT( slotApplyConfig() ) );

    configDlg.show();
}

void KNote::slotToDesktop( int id )
{
    if ( id == 0 || id == NETWinInfo::OnAllDesktops )
        KWin::setOnAllDesktops( winId(), true );
    else
        KWin::setOnDesktop( winId(), id );
}

void KNote::slotAlwaysOnTop()
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
    for ( int i = 1; i <= num; i++ )
    {
        desktops.append( QString::fromUtf8(wm_root.desktopName( i )) );
    }

    //fill m_desktop_menu with the available desktop names
    bool alldesktops = false;
    int note_desktop = wm_client.desktop();

    if ( note_desktop == NETWinInfo::OnAllDesktops )
        alldesktops = true;

    m_desktop_menu->clear();
    m_desktop_menu->insertItem( i18n( "&All desktops" ), 0 );
    if ( alldesktops )
        m_desktop_menu->setItemChecked( 0, true );
    m_desktop_menu->insertSeparator( -1 );

    uint id;
    QStringList::Iterator it = desktops.begin();
    int num_desktops = (int)desktops.count();
    for ( int i = 1; i <= num_desktops; i++, it++ )
    {
        id = m_desktop_menu->insertItem( QString("&%1 %2").arg(i).arg( *it ), i );
        if ( note_desktop == i )
        {
            m_desktop_menu->setItemChecked( id, true );
        }
    }
}

void KNote::slotMail() const
{
    saveData();
    KSimpleConfig config( m_noteDir.absFilePath( m_configFile ), true );

    //sync up the data on note and the data file
    QString msg_body = m_noteDir.absFilePath( "." + m_configFile + "_data" );

    //get the mail action command
    config.setGroup( "Actions" );
    QString mail_cmd = config.readEntry( "mail", "kmail --msg %f" );
    QStringList cmd_list = QStringList::split( QChar(' '), mail_cmd );

    KProcess mail;
    for ( QStringList::Iterator it = cmd_list.begin();
        it != cmd_list.end(); ++it )
    {
        if ( *it == "%f" )
            mail << msg_body;
        else
            mail << *it;
    }

    if ( !mail.start( KProcess::DontCare ) )
        kdDebug() << "could not start process" << endl;
}

void KNote::slotPrint() const
{
    saveData();

    KSimpleConfig config( m_noteDir.absFilePath( m_configFile ), true );
    KPrinter printer;

    if ( printer.setup() )
    {
        config.setGroup( "Actions" );
        QString printstr = config.readEntry(
                              "print",
                              "a2ps -P %p -1 --center-title=%t --underlay=KDE %f"
                           );

        QString printername = printer.printerName();
        QString title = m_label->text();
        QString datafile = m_noteDir.absFilePath( "." + m_configFile + "_data" );

        QStringList cmd_list = QStringList::split( QChar(' '), printstr );
        KProcess printjob;
        for ( QStringList::Iterator it = cmd_list.begin();
            it != cmd_list.end(); ++it )
        {
            if ( *it == "%p" )
                printjob << printername;
            else if ( *it == "%t" )
                printjob << title;
            else if ( *it == "%f" )
                printjob << datafile;
            else
                printjob << *it;
        }

        bool result = printjob.start( KProcess::Block, KProcess::NoCommunication );
        if ( !result )
            kdDebug() << "printing failed" << endl;
    }
}


// -------------------- private slots -------------------- //

void KNote::slotApplyConfig()
{
    KSimpleConfig config( m_noteDir.absFilePath( m_configFile ) );

    //do the Editor group: tabsize, autoindent, font, fontsize, fontstyle
    config.setGroup( "Editor" );

    QFont def( "helvetica" );
    def = config.readFontEntry( "font", &def );
    m_editor->setTextFont( def );

    def = QFont( "helvetica" );
    def = config.readFontEntry( "titlefont", &def );
    m_label->setFont( def );

    uint tab_size = config.readUnsignedNumEntry( "tabsize", 4 );
//    m_editor->setTabStopWidth( tab_size );
    m_editor->setTabStops( tab_size );

    bool indent = true;
    indent = config.readBoolEntry( "autoindent", &indent );
    m_editor->setAutoIndentMode( indent );

    //do the Data Group- name, data
    config.setGroup( "Data" );

    // TODO
    if ( m_label->text().isEmpty() )
    {
        QString notename = config.readEntry( "name", m_configFile );
        m_label->setText( notename );
    }

    // do Display group - bgcolor, fgcolor, transparent
    config.setGroup( "Display" );

    // create a pallete...
    QColor bg = config.readColorEntry( "bgcolor", &(Qt::yellow) );
    QColor fg = config.readColorEntry( "fgcolor", &(Qt::black) );

    QPalette newpalette = palette();
    newpalette.setColor( QColorGroup::Background, bg );
    newpalette.setColor( QColorGroup::Foreground, fg );
    newpalette.setColor( QColorGroup::Base,       bg ); // text background
    newpalette.setColor( QColorGroup::Text,       fg ); // text color

    // the shadow
    newpalette.setColor( QColorGroup::Midlight, bg.light(110) );
    newpalette.setColor( QColorGroup::Shadow, bg.dark(116) );
    newpalette.setColor( QColorGroup::Light, bg.light(180) );
    newpalette.setColor( QColorGroup::Dark, bg.dark(108) );
    setPalette( newpalette );

    // set darker values for the label and button...
    m_button->setBackgroundColor( bg.dark(116) );
    if ( hasFocus() )
    {
        m_label->setBackgroundColor( bg.dark(116) );
        m_button->show();
    }
    else
    {
        m_label->setBackgroundColor( bg );
        m_button->hide();
    }
}


// -------------------- private methods -------------------- //

void KNote::convertOldConfig()
{
    QFile infile( m_noteDir.absFilePath( m_configFile ) );

    if ( infile.open( IO_ReadOnly ) )
    {
        QTextStream input( &infile );

        // get the name
        m_label->setText( input.readLine() );

        // get the geometry
        QString geo = input.readLine();

        int pos, data[13];
        int n = 0;

        while ( (pos = geo.find('+')) != -1 )
        {
            if( n < 13 )
                data[n++] = geo.left(pos).toInt();
            geo.remove( 0, pos + 1 );
        }
        if ( n < 13 )
            data[n++] = geo.toInt();

        int note_desktop = data[0];
        if ( data[11] == 1 )
            note_desktop = NETWinInfo::OnAllDesktops;

        resize( data[3], data[4] );
        if ( data[1] >= 0 && data[2] >= 0 )   // just to be sure...
            move( data[1], data[2] );

        if ( data[12] & 2048 )
        {
            KWin::setState( winId(), NET::StaysOnTop | NET::SkipTaskbar );
            m_menu->setItemChecked( m_idAlwaysOnTop, true );
        }
        else
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
        newpalette.setColor( QColorGroup::Foreground, fg );
        newpalette.setColor( QColorGroup::Base,       bg ); // text background
        newpalette.setColor( QColorGroup::Text,       fg ); // text color

        // the shadow
        newpalette.setColor( QColorGroup::Midlight, bg.light(110) );
        newpalette.setColor( QColorGroup::Shadow, bg.dark(116) );
        newpalette.setColor( QColorGroup::Light, bg.light(180) );
        newpalette.setColor( QColorGroup::Dark, bg.dark(108) );
        setPalette( newpalette );

        // set darker values for the label and button...
        m_button->setBackgroundColor( bg.dark(116) );
        if ( hasFocus() )
        {
            m_label->setBackgroundColor( bg.dark(116) );
            m_button->show();
        }
        else
        {
            m_label->setBackgroundColor( bg );
            m_button->hide();
        }

        // get the font
        QString fontfamily = input.readLine();
        if ( fontfamily.isEmpty() )
            fontfamily = QString( "helvetica" );
        uint size = input.readLine().toUInt();
        size = QMAX( size, 4 );
        uint weight = input.readLine().toUInt();
        bool italic = ( input.readLine().toUInt() == 1 );

        QFont font( fontfamily, size, weight, italic );
        m_label->setFont( font );
        m_editor->setTextFont( font );

        // 3d frame? Not supported yet!
        input.readLine();

        // autoindent
        bool indent = ( input.readLine().toUInt() == 1 );
        m_editor->setAutoIndentMode( indent );
//        m_editor->setTabStopWidth( 4 );
        m_editor->setTabStops( 4 );

        // hidden
        bool hidden = ( input.readLine().toUInt() == 1 );

        // show the note
        if ( !hidden && !isVisible() )
        {
            // HACK HACK
            if ( note_desktop != NETWinInfo::OnAllDesktops )
            {
                // to avoid flicker, call this before show()
                slotToDesktop( note_desktop );
                show();
            } else {
                show();
                // if this is called before show(), it won't work for sticky notes!!!
                slotToDesktop( note_desktop );
            }
        }

        // get the text
        while ( !input.atEnd() )
            m_editor->insert( input.readLine() + "\n" );
//          m_editor->insertParagraph( input.readLine(), -1 );

        infile.close();
        infile.remove();     // TODO: success?

        // set the new configfile's name...
        for ( int i = 1; ; i++ )
        {
            m_configFile = QString( "KNote %1" ).arg(i);
            if ( !m_noteDir.exists( m_configFile ) )
                break;
        }

        // write the new configuration
        KIO::NetAccess::copy(
            KURL( KGlobal::dirs()->findResource( "config", "knotesrc" ) ),
            KURL( m_noteDir.absFilePath( m_configFile ) )
        );

        saveData();
        saveConfig();
        saveDisplayConfig();

        // TODO: Needed? What about KConfig?
        KSimpleConfig config( m_noteDir.absFilePath( m_configFile ) );
        config.setGroup( "General" );
        config.writeEntry( "version", 2 );

        config.setGroup( "Display" );
        config.writeEntry( "fgcolor", fg );
        config.writeEntry( "bgcolor", bg );

        config.setGroup( "Actions" );      // use the new default for this group
        config.writeEntry( "mail", "kmail --msg %f" );
        config.writeEntry( "print", "a2ps -P %p -1 --center-title=%t --underlay=KDE %f" );

        config.setGroup( "Editor" );
        config.writeEntry( "autoindent", indent );
        config.writeEntry( "titlefont", font );
        config.writeEntry( "font", font );
        config.writeEntry( "tabsize", 4 );
        config.sync();
    } else
        kdDebug() << "could not open input file" << endl;
}


// -------------------- protected methods -------------------- //

void KNote::resizeEvent( QResizeEvent* qre )
{
    QFrame::resizeEvent( qre );

    int headerHeight = m_label->sizeHint().height();
    m_label->setFixedHeight( headerHeight );
    m_button->setFixedSize( headerHeight, headerHeight );

    m_button->setGeometry( frameRect().width() - headerHeight - 2, frameRect().y() + 2,
                headerHeight, headerHeight );
    m_label->setGeometry( frameRect().x() + 2, frameRect().y() + 2,
                frameRect().width() - headerHeight - 4, headerHeight );
    m_editor->setGeometry( contentsRect().x(), contentsRect().y() + headerHeight + 2,
                contentsRect().width(), contentsRect().height() - headerHeight - 4 );
}

void KNote::closeEvent( QCloseEvent* e )
{
    saveConfig();
    saveDisplayConfig();

    QWidget::closeEvent( e );
}

void KNote::keyPressEvent( QKeyEvent* e )
{
    if ( e->key() == Key_Escape )
        slotClose();
    else
        e->ignore();
}

bool KNote::eventFilter( QObject* o, QEvent* ev )
{
    if ( o == m_label )
    {
        QMouseEvent* e = (QMouseEvent*)ev;

        if ( ev->type() == QEvent::MouseButtonRelease )
        {
            if ( e->button() == LeftButton )
            {
                m_dragging = false;
                m_label->releaseMouse();
                raise();
            }
            if ( e->button() == MidButton )
                lower();
            return true;
        }

        if ( ev->type() == QEvent::MouseButtonPress && e->button() == LeftButton )
        {
            m_pointerOffset = e->pos();
            m_label->grabMouse( sizeAllCursor );
            return true;
        }
        if ( ev->type() == QEvent::MouseMove && m_label == mouseGrabber())
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

        if ( m_menu && ( ev->type() == QEvent::MouseButtonPress )
            && ( e->button() == RightButton ) )
        {
            m_menu->popup( QCursor::pos() );
            return true;
        }

        return m_label->eventFilter( o, ev );
    }
    else if ( o == m_editor )
    {
        if ( ev->type() == QEvent::FocusOut )
        {
            m_label->setBackgroundColor( palette().active().background() );
            m_button->hide();

            if ( m_editor->isModified() )
                saveData();
        }
        else if ( ev->type() == QEvent::FocusIn )
        {
            m_label->setBackgroundColor( palette().active().shadow() );
            m_button->show();
        }
        return false;
    }
    return QWidget::eventFilter( o, ev );
}

#include "knote.moc"
#include "knotebutton.moc"
