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

#include <qlabel.h>
#include <qsizegrip.h>
#include <qbitmap.h>
#include <qcursor.h>
#include <qpointarray.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kaction.h>
#include <kxmlguifactory.h>
#include <kprinter.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <klineeditdlg.h>
#include <kpopupmenu.h>
#include <kmdcodec.h>
#include <kglobalsettings.h>

#include "libkcal/journal.h"

#include "knote.h"
#include "knotebutton.h"
#include "knoteedit.h"
#include "knoteconfigdlg.h"
#include "version.h"

#include <kwin.h>
#include <netwm.h>

// fscking X headers
#ifdef FocusIn
#undef FocusIn
#endif
#ifdef FocusOut
#undef FocusOut
#endif

extern Atom qt_sm_client_id;

using namespace KCal;


KNote::KNote( KXMLGUIBuilder* builder, QDomDocument buildDoc, Journal *j,
              QWidget* parent, const char* name )
  : QFrame( parent, name, WStyle_Customize | WStyle_NoBorder | WDestructiveClose ),
    m_journal( j )
{
    // to disable kwin's session management (ie. saving positions of windows) we need to
    // remove the session id from all note windows
    XChangeProperty( x11Display(), winId(), qt_sm_client_id, XA_STRING, 8,
        PropModeReplace, 0, 0 );

    // create the menu items for the note - not the editor...
    // rename, mail, print, insert date, close, delete, new note
    new KAction( i18n("New"), "filenew", 0, this, SLOT(slotNewNote()), actionCollection(), "new_note" );
    new KAction( i18n("Rename..."), "text", 0, this, SLOT(slotRename()), actionCollection(), "rename_note" );
    new KAction( i18n("Hide"), "fileclose" , 0, this, SLOT(slotClose()), actionCollection(), "hide_note" );
    new KAction( i18n("Delete"), "knotes_delete", 0, this, SLOT(slotKill()), actionCollection(), "delete_note" );

    new KAction( i18n("Insert Date"), "knotes_date", 0 , this, SLOT(slotInsDate()), actionCollection(), "insert_date" );
    new KAction( i18n("Mail..."), "mail_send", 0, this, SLOT(slotMail()), actionCollection(), "mail_note" );
    new KAction( i18n("Print..."), "fileprint", 0, this, SLOT(slotPrint()), actionCollection(), "print_note" );
    new KAction( i18n("Preferences..."), "configure", 0, this, SLOT(slotPreferences()), actionCollection(), "configure_note" );

    m_alwaysOnTop = new KToggleAction( i18n("Always on Top"), "attach", 0, this, SLOT(slotToggleAlwaysOnTop()), actionCollection(), "always_on_top" );
    connect( m_alwaysOnTop, SIGNAL(toggled(bool)), m_alwaysOnTop, SLOT(setChecked(bool)) );
    m_toDesktop = new KListAction( i18n("To Desktop"), 0, this, SLOT(slotPopupActionToDesktop(int)), actionCollection(), "to_desktop" );
    connect( m_toDesktop->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(slotUpdateDesktopActions()) );

    // create the note header, button and label...
    m_button = new KNoteButton( "knotes_close", this );
    connect( m_button, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

    m_label = new QLabel( this );
    m_label->setAlignment( AlignHCenter );
    m_label->installEventFilter( this );  // recieve events (for dragging & action menu)
    m_label->setText( m_journal->summary() );

    // create the toolbar
    m_tool = new QWidget( this, "toolbar" );
    m_tool->hide();

    // create the note editor
    m_editor = new KNoteEdit( m_tool, this );
    m_editor->installEventFilter( this ); // recieve events (for modified)
    m_editor->viewport()->installEventFilter( this );

    setDOMDocument( buildDoc );
    factory = new KXMLGUIFactory( builder, this, "guifactory" );
    factory->addClient( this );

    m_menu = static_cast<KPopupMenu*>(factory->container( "note_context", this ));
    m_edit_menu = static_cast<KPopupMenu*>(factory->container( "note_edit", this ));

    setFocusProxy( m_editor );

    // create the resize handle
    m_editor->setCornerWidget( new QSizeGrip( this ) );
    uint width = m_editor->cornerWidget()->width();
    uint height = m_editor->cornerWidget()->height();
    QBitmap mask;
    mask.resize( width, height );
    mask.fill( color0 );
    QPointArray array;
    array.setPoints( 3, 0, height, width, height, width, 0 );
    QPainter p;
    p.begin( &mask );
    p.setBrush( color1 );
    p.drawPolygon( array );
    p.end();
    m_editor->cornerWidget()->setMask( mask );

    // set up the look&feel of the note
    setMinimumSize( 20, 20 );
    setFrameStyle( WinPanel | Raised );
    setLineWidth( 1 );

    m_editor->setMargin( 5 );
    m_editor->setFrameStyle( NoFrame );
    m_editor->setBackgroundMode( PaletteBase );

    // get the config attachment
    m_configFile = m_journal->attachments(CONFIG_MIME).first()->uri();

    // load the display configuration of the note
    KSimpleConfig config( m_configFile );
    config.setGroup( "Display" );
    width  = config.readUnsignedNumEntry( "width", 200 );
    height = config.readUnsignedNumEntry( "height", 200 );
    resize( width, height );

    config.setGroup( "WindowDisplay" );
    int note_desktop = config.readNumEntry( "desktop", KWin::currentDesktop() );
    ulong note_state = config.readUnsignedLongNumEntry( "state", NET::SkipTaskbar );
    QPoint default_position = QPoint( -1, -1 );
    QPoint position  = config.readPointEntry( "position", &default_position );

    KWin::setState( winId(), note_state );
    if ( note_state & NET::StaysOnTop )
        m_alwaysOnTop->setChecked( true );

    // let KWin do the placement if the position is illegal
    if ( kapp->desktop()->rect().contains( position.x()+width, position.y()+height ) && position != default_position )
        move( position );           // do before calling show() to avoid flicker

    // read configuration settings...
    slotApplyConfig();

    // show the note if desired
    if ( note_desktop != 0 && !isVisible() )
    {
        // HACK HACK
        if( note_desktop != NETWinInfo::OnAllDesktops )
        {
            // to avoid flicker, call this before show()
            toDesktop( note_desktop );
            show();
        } else {
            show();
            // if this is called before show(),
            // it won't work for sticky notes!!!
            toDesktop( note_desktop );
        }
    }

    m_editor->setText( m_journal->description() );
    m_editor->setModified( false );
}

KNote::~KNote()
{
kdDebug(5500) << k_funcinfo << endl;
}


// -------------------- public member functions -------------------- //

void KNote::saveData()
{
    m_journal->setSummary( m_label->text() );
    m_journal->setDescription( m_editor->text() );

    // TODO: call m_calendar.update( this ) in knotesapp?
    emit sigDataChanged();
    m_editor->setModified( false );
}

void KNote::saveConfig() const
{
    // all that needs to get saved here is the size and name
    // everything else would have been saved by the preferences dialog
    KSimpleConfig config( m_configFile );

    // need to save the new size to KSimpleConfig object
    // but don't save the height of the toolbar
    config.setGroup( "Display" );
    config.writeEntry( "width", width() );
    config.writeEntry( "height", height() - (m_tool->isHidden() ? 0:m_tool->height()) );

    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop | NET::WMState );
    config.setGroup( "WindowDisplay" );
    config.writeEntry( "desktop", wm_client.desktop() );
    config.writeEntry( "state", wm_client.state() );
    // TODO: move to group Display
    config.writeEntry( "position", pos() );
}

QString KNote::noteId() const
{
    return m_journal->uid();
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
    saveData();
}

void KNote::setText( const QString& text )
{
    m_editor->setText( text );
    saveData();
}

void KNote::sync( const QString& app )
{
    QByteArray sep( 1 );
    sep[0] = '\0';

    KMD5 hash;
    QCString result;

    hash.update( m_label->text().utf8() );
    hash.update( sep );
    hash.update( m_editor->text().utf8() );
    hash.hexDigest( result );

    KSimpleConfig config( m_configFile );

    config.setGroup( "Synchronisation" );
    config.writeEntry( app, result.data() );
}

bool KNote::isNew( const QString& app ) const
{
    KSimpleConfig config( m_configFile );

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
    hash.hexDigest();

    KSimpleConfig config( m_configFile );
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
    // pop up dialog to get the new name
    bool ok;
    QString newName = KLineEditDlg::getText( i18n("Please enter the new name:"),
                                             m_label->text(), &ok, this );
    if ( !ok ) // handle cancel
        return;

    setName( newName );
    emit sigConfigChanged();
}

void KNote::slotClose()
{
    m_editor->clearFocus();
    hide(); //just hide the note so it's still available from the dock window
}

void KNote::slotKill()
{
    if ( KMessageBox::warningYesNo( this,
         i18n("<qt>Do you really want to delete note <b>%1</b>?</qt>").arg( m_label->text() ),
         i18n("Confirm Delete") ) == KMessageBox::Yes )
    {
        emit sigKillNote( m_journal );
    }
}

void KNote::slotInsDate()
{
    m_editor->insert( KGlobal::locale()->formatDateTime(QDateTime::currentDateTime()) );
}

void KNote::slotPreferences()
{
    // launch preferences dialog...
    KNoteConfigDlg configDlg( m_configFile, i18n("Local Settings"), false );
    connect( &configDlg, SIGNAL( updateConfig() ), this, SLOT( slotApplyConfig() ) );
    configDlg.exec();
}

void KNote::slotToggleAlwaysOnTop()
{
    if ( KWin::info(winId()).state & NET::StaysOnTop )
        KWin::clearState( winId(), NET::StaysOnTop );
    else
        KWin::setState( winId(), KWin::info(winId()).state | NET::StaysOnTop );
}

void KNote::slotPopupActionToDesktop( int id )
{
    if( id > 1 )
	--id; // compensate for the menu separator
    toDesktop( id );
}

void KNote::toDesktop( int desktop )
{
    if ( desktop == 0 || desktop == NETWinInfo::OnAllDesktops )
        KWin::setOnAllDesktops( winId(), true );
    else
        KWin::setOnDesktop( winId(), desktop );
}

void KNote::slotUpdateDesktopActions()
{
    NETRootInfo wm_root( qt_xdisplay(), NET::NumberOfDesktops | NET::DesktopNames );
    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop );

    QStringList desktops;
    desktops.append( i18n("&All Desktops") );
    desktops.append( QString::null );           // Separator

    int count = wm_root.numberOfDesktops();
    for ( int n = 1; n <= count; n++ )
        desktops.append( QString("&%1 %2").arg( n ).arg( QString::fromUtf8(wm_root.desktopName( n )) ) );

    m_toDesktop->setItems( desktops );

    kdDebug() << "updateDesktopActions:" << wm_client.desktop() << endl;
    if ( wm_client.desktop() == NETWinInfo::OnAllDesktops )
        m_toDesktop->setCurrentItem( 0 );
    else
        m_toDesktop->setCurrentItem( wm_client.desktop() + 1 ); // compensate for separator (+1)
}

void KNote::slotMail() //const
{
    saveData();
    KSimpleConfig config( m_configFile, true );

    // TODO: convert to plain text
    QString msg_body = m_editor->text();

    //get the mail action command
    config.setGroup( "Actions" );
    QString mail_cmd = config.readEntry( "mail", "kmail --msg %f" );
    QStringList cmd_list = QStringList::split( QChar(' '), mail_cmd );

    KProcess mail;
    for ( QStringList::Iterator it = cmd_list.begin();
        it != cmd_list.end(); ++it )
    {
        if ( *it == "%f" )
            mail << msg_body.local8Bit();
        else if ( *it == "%t" )
            mail << m_label->text().local8Bit();
        else
            mail << (*it).local8Bit();
    }

    if ( !mail.start( KProcess::DontCare ) )
    {
        KMessageBox::sorry( this, i18n("Unable to start the mail process.") );
    }
}

void KNote::slotPrint()
{
    saveData();

    KPrinter printer;
    printer.setFullPage( true );

    if ( printer.setup() )
    {
        KSimpleConfig config( m_configFile, true );
        config.setGroup( "Editor" );

        QFont font( KGlobalSettings::generalFont() );
        font = config.readFontEntry( "font", &font );

        QPainter painter;
        painter.begin( &printer );

        const int margin = 40;  // pt

        QPaintDeviceMetrics metrics( painter.device() );
        int marginX = margin * metrics.logicalDpiX() / 72;
        int marginY = margin * metrics.logicalDpiY() / 72;

        QRect body( marginX, marginY,
                    metrics.width() - marginX * 2,
                    metrics.height() - marginY * 2 );

        QString content;
        if ( m_editor->textFormat() == PlainText )
            content = QStyleSheet::convertFromPlainText( m_editor->text() );
        else
            content = m_editor->text();

        QSimpleRichText text( content, font, m_editor->context(),
                              m_editor->styleSheet(), m_editor->mimeSourceFactory(),
                              body.height() /*, linkColor, linkUnderline? */ );

        text.setWidth( &painter, body.width() );
        QRect view( body );

        int page = 1;

        for (;;) {
            text.draw( &painter, body.left(), body.top(), view, colorGroup() );
            view.moveBy( 0, body.height() );
            painter.translate( 0, -body.height() );

            // page numbers
            painter.setFont( font );
            painter.drawText(
                view.right() - painter.fontMetrics().width( QString::number( page ) ),
                view.bottom() + painter.fontMetrics().ascent() + 5, QString::number( page )
            );

            if ( view.top() >= text.height() )
                break;

            printer.newPage();
            page++;
        }

        painter.end();
    }
}


// -------------------- private slots -------------------- //

void KNote::slotApplyConfig()
{
    KSimpleConfig config( m_configFile );

    // do the Editor group - tabsize, autoindent, textformat, font, fontsize, fontstyle
    config.setGroup( "Editor" );

    bool richtext = config.readBoolEntry( "richtext", false );
    if ( richtext )
        m_editor->setTextFormat( RichText );
    else
    {
        m_editor->setTextFormat( PlainText );
        m_editor->setText( m_editor->text() );
    }

    QFont def( KGlobalSettings::generalFont() );
    def = config.readFontEntry( "font", &def );
    m_editor->setTextFont( def );

    // TODO remove this!
    def = config.readFontEntry( "titlefont", &def );
    m_label->setFont( def );

    uint tab_size = config.readUnsignedNumEntry( "tabsize", 4 );
    m_editor->setTabStop( tab_size );

    bool indent = config.readBoolEntry( "autoindent", true );
    m_editor->setAutoIndentMode( indent );

    // do Display group - bgcolor, fgcolor, transparent
    config.setGroup( "Display" );

    // create a pallete...
    QColor bg = config.readColorEntry( "bgcolor", &(Qt::yellow) );
    QColor fg = config.readColorEntry( "fgcolor", &(Qt::black) );

    setColor( fg, bg );

    emit sigConfigChanged();
}


// -------------------- private methods -------------------- //

void KNote::setColor( const QColor &fg, const QColor &bg )
{
    QPalette newpalette = palette();
    newpalette.setColor( QColorGroup::Background, bg );
    newpalette.setColor( QColorGroup::Foreground, fg );
    newpalette.setColor( QColorGroup::Base,       bg ); // text background
    newpalette.setColor( QColorGroup::Text,       fg ); // text color
    newpalette.setColor( QColorGroup::Button,     bg );

    // the shadow
    newpalette.setColor( QColorGroup::Midlight, bg.light(110) );
    newpalette.setColor( QColorGroup::Shadow, bg.dark(116) );
    newpalette.setColor( QColorGroup::Light, bg.light(180) );
    newpalette.setColor( QColorGroup::Dark, bg.dark(108) );
    setPalette( newpalette );

    // set the text color
    m_editor->setTextColor( fg );

    // set darker value for the hide button...
    QPalette darker = palette();
    darker.setColor( QColorGroup::Button, bg.dark(116) );
    m_button->setPalette( darker );

    // to set the color of the title
    updateFocus();
}

void KNote::updateFocus()
{
kdDebug(5500) << k_funcinfo << endl;
    if ( hasFocus() )
    {
        m_label->setBackgroundColor( palette().active().shadow() );
        m_button->show();
        m_editor->cornerWidget()->show();

        if ( m_tool->isHidden() && m_editor->textFormat() == QTextEdit::RichText )
        {
            m_tool->show();
            setGeometry( x(), y(), width(), height() + m_tool->height() );
        }
    }
    else
    {
        m_label->setBackgroundColor( palette().active().background() );
        m_button->hide();
        m_editor->cornerWidget()->hide();

        if ( !m_tool->isHidden() )
        {
            m_tool->hide();
            updateLayout();     // to update the minimum height
            setGeometry( x(), y(), width(), height() - m_tool->height() );
        }
    }
}

void KNote::updateLayout()
{
kdDebug(5500) << k_funcinfo << endl;
    // DAMN, Qt 3.1 still has no support for widgets with a fixed aspect ratio :-(
    // So we have to write our own layout manager...

    int headerHeight = m_label->sizeHint().height();
    int margin = m_editor->margin();

    m_button->setGeometry(
                frameRect().width() - headerHeight - 2,
                frameRect().y() + 2,
                headerHeight,
                headerHeight
             );

    m_label->setGeometry(
                frameRect().x() + 2,
                frameRect().y() + 2,
                frameRect().width() - (m_button->isHidden()?0:headerHeight) - 4,
                headerHeight
             );

    m_tool->setGeometry(
                contentsRect().x(),
                contentsRect().y() + headerHeight + 2,
                contentsRect().width(),
                16
                //m_tool->minimumSizeHint().height()
             );

    int toolHeight = m_tool->isHidden() ? 0 : m_tool->height();

    m_editor->setGeometry(
                contentsRect().x(),
                contentsRect().y() + headerHeight + toolHeight + 2,
                contentsRect().width(),
                contentsRect().height() - headerHeight - toolHeight - 4
             );

    setMinimumSize( m_editor->cornerWidget()->width() + margin*2 + 4,
                    headerHeight + toolHeight + m_editor->cornerWidget()->height() + margin*2 + 4 );
}

// -------------------- protected methods -------------------- //

void KNote::resizeEvent( QResizeEvent* qre )
{
    QFrame::resizeEvent( qre );
    updateLayout();
}

void KNote::closeEvent( QCloseEvent* /*e*/ )
{
    slotClose();
}

void KNote::keyPressEvent( QKeyEvent* e )
{
    if ( e->key() == Key_Escape )
        slotClose();
    else
        e->ignore();
}

bool KNote::event( QEvent* ev )
{
    if ( ev->type() == QEvent::LayoutHint )
    {
        updateLayout();
        return true;
    }
    else
        return QFrame::event( ev );
}

bool KNote::eventFilter( QObject* o, QEvent* ev )
{
    if ( o == m_label )
    {
        QMouseEvent* e = (QMouseEvent*)ev;

        if ( ev->type() == QEvent::MouseButtonDblClick )
            slotRename();

        if ( ev->type() == QEvent::MouseButtonRelease &&
             (e->button() == LeftButton || e->button() == MidButton) )
        {
            m_dragging = false;
            m_label->releaseMouse();
            return true;
        }

        if ( ev->type() == QEvent::MouseButtonPress &&
             (e->button() == LeftButton || e->button() == MidButton))
        {
            m_pointerOffset = e->pos();
            m_label->grabMouse( sizeAllCursor );

            e->button() == LeftButton ? raise() : lower();

            return true;
        }

        if ( ev->type() == QEvent::MouseMove && m_label == mouseGrabber() )
        {
            if ( m_dragging )
                move( QCursor::pos() - m_pointerOffset );
            else
            {
                m_dragging = (
                    (e->pos().x() - m_pointerOffset.x()) *
                    (e->pos().x() - m_pointerOffset.x())
                    +
                    (e->pos().y() - m_pointerOffset.y()) *
                    (e->pos().y() - m_pointerOffset.y())   >= 9
                );
            }
            return true;
        }

        if ( m_menu && ( ev->type() == QEvent::MouseButtonPress )
            && ( e->button() == RightButton ) )
        {
            m_menu->popup( QCursor::pos() );
            return true;
        }

        return false;
    }

    if ( o == m_editor )
    {
        if ( ev->type() == QEvent::FocusOut )
        {
            if ( static_cast<QFocusEvent*>(ev)->reason() != QFocusEvent::Popup )
                updateFocus();
            if ( m_editor->isModified() )
                saveData();
        }
        else if ( ev->type() == QEvent::FocusIn )
            updateFocus();

        return false;
    }

    if ( o == m_editor->viewport() )
    {
        if ( ev->type() == QEvent::MouseButtonPress )
            if ( m_edit_menu && ((QMouseEvent*)ev)->button() == RightButton )
            {
                m_edit_menu->popup( QCursor::pos() );
                return true;
            }
    }

    return false;
}

#include "knote.moc"
#include "knotebutton.moc"
