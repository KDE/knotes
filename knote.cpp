/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2004, The KNotes Developers

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
#include <qsize.h>
#include <qsizegrip.h>
#include <qbitmap.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>
#include <qobjectlist.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kcombobox.h>
#include <ktoolbar.h>
#include <kpopupmenu.h>
#include <kxmlguifactory.h>
#include <kprinter.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kinputdialog.h>
#include <kmdcodec.h>
#include <kglobalsettings.h>
#include <kio/netaccess.h>

#include <libkcal/journal.h>

#include "knote.h"
#include "knotebutton.h"
#include "knoteedit.h"
#include "knoteconfig.h"
#include "knotesglobalconfig.h"
#include "knoteconfigdlg.h"
#include "knotehostdlg.h"
#include "knotesnetsend.h"
#include "version.h"

#include <kwin.h>
#include <netwm.h>
//#include <kdecoration.h>

#include <fixx11h.h>

using namespace KCal;


KNote::KNote( KXMLGUIBuilder* builder, QDomDocument buildDoc, Journal *j,
              QWidget* parent, const char* name )
  : QFrame( parent, name, WStyle_Customize | WStyle_NoBorder | WDestructiveClose ),
    m_label( 0 ), m_button( 0 ), m_tool( 0 ), m_editor( 0 ),
    m_config( 0 ), m_journal( j )
{
    //actionCollection()->setWidget( this );

    // if there is no title yet, use the start date if valid
    // (KOrganizer's journals don't have titles but a valid start date)
    if ( m_journal->summary().isNull() && m_journal->dtStart().isValid() )
    {
        QString s = KGlobal::locale()->formatDateTime( m_journal->dtStart() );
        m_journal->setSummary( s );
    }

    // create the menu items for the note - not the editor...
    // rename, mail, print, insert date, close, delete, new note
    new KAction( i18n("New"), "filenew", 0,
        this, SIGNAL(sigRequestNewNote()), actionCollection(), "new_note" );
    new KAction( i18n("Rename..."), "text", 0,
        this, SLOT(slotRename()), actionCollection(), "rename_note" );
    new KAction( i18n("Hide"), "fileclose" , 0,
        this, SLOT(slotClose()), actionCollection(), "hide_note" );
    new KAction( i18n("Delete"), "knotes_delete", 0,
        this, SLOT(slotKill()), actionCollection(), "delete_note" );

    new KAction( i18n("Insert Date"), "knotes_date", 0 ,
        this, SLOT(slotInsDate()), actionCollection(), "insert_date" );
    new KAction( i18n("Send..."), "network", 0,
        this, SLOT(slotSend()), actionCollection(), "send_note" );
    new KAction( i18n("Mail..."), "mail_send", 0,
        this, SLOT(slotMail()), actionCollection(), "mail_note" );
    KStdAction::print( this, SLOT(slotPrint()), actionCollection(), "print_note" );
    new KAction( i18n("Preferences..."), "configure", 0,
        this, SLOT(slotPreferences()), actionCollection(), "configure_note" );

    m_keepAbove = new KToggleAction( i18n("Keep Above Others"), "up", 0,
        this, SLOT(slotUpdateKeepAboveBelow()), actionCollection(), "keep_above" );
    m_keepAbove->setExclusiveGroup( "keepAB" );

    m_keepBelow = new KToggleAction( i18n("Keep Below Others"), "down", 0,
        this, SLOT(slotUpdateKeepAboveBelow()), actionCollection(), "keep_below" );
    m_keepBelow->setExclusiveGroup( "keepAB" );

    m_toDesktop = new KListAction( i18n("To Desktop"), 0,
        this, SLOT(slotPopupActionToDesktop(int)), actionCollection(), "to_desktop" );
    connect( m_toDesktop->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(slotUpdateDesktopActions()) );

    // create the note header, button and label...
    m_button = new KNoteButton( "knotes_close", this );
    connect( m_button, SIGNAL(clicked()), this, SLOT(slotClose()) );

    m_label = new QLabel( this );
    m_label->installEventFilter( this );  // receive events (for dragging & action menu)
    setName( m_journal->summary() );      // don't worry, no signals are connected at this stage yet

    // create the note editor
    m_editor = new KNoteEdit( this );
    m_editor->installEventFilter( this ); // receive events (for modified)
    m_editor->viewport()->installEventFilter( this );

    setDOMDocument( buildDoc );
    KXMLGUIFactory factory( builder, this, "guifactory" );
    factory.addClient( this );

    m_menu = static_cast<KPopupMenu*>(factory.container( "note_context", this ));
    m_edit_menu = static_cast<KPopupMenu*>(factory.container( "note_edit", this ));
    m_tool = static_cast<KToolBar*>(factory.container( "note_tool", this ));
    m_tool->reparent( this, QPoint( 0, 0 ) );
    m_tool->hide();

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

    // the config file location
    QString configFile = KGlobal::dirs()->saveLocation( "appdata", "notes/" );
    configFile += m_journal->uid();
    KURL dst( configFile );

    // no config file yet? -> use the default display config if available
    // we want to write to configFile, so use "false"
    if ( !KIO::NetAccess::exists( dst, false, 0 ) )
    {
        // use saveLocation since it's the user's default and not the
        // system's default (i.e., KNotes has to have write permission)
        KURL src( KGlobal::dirs()->saveLocation( "config" ) + "knotesrc" );

        // "fill" the config file with the default config
        if ( KIO::NetAccess::exists( src, true, 0 ) )
            KIO::NetAccess::file_copy( src, dst, -1, true, false, 0 );
    }

    m_config = new KNoteConfig( KSharedConfig::openConfig( configFile, false, false ) );
    m_config->readConfig();
    m_config->setVersion( KNOTES_VERSION );

    // load the display configuration of the note
    width = m_config->width();
    height = m_config->height();
    resize( width, height );

    if ( m_config->keepAbove() )
        m_keepAbove->setChecked( true );
    else if ( m_config->keepBelow() )
        m_keepBelow->setChecked( true );
    else
    {
        m_keepAbove->setChecked( false );
        m_keepBelow->setChecked( false );
    }

    // let KWin do the placement if the position is illegal
    const QPoint& position = m_config->position();
    if ( kapp->desktop()->rect().intersects( QRect( position, QSize( width, height ) ) ) )
        move( position );           // do before calling show() to avoid flicker

    // read configuration settings...
    slotApplyConfig();

    // if this is a new note put on current desktop - we can't use defaults
    // in KConfig XT since only _changes_ will be stored in the config file
    int desktop = m_config->desktop();
    if ( desktop < 0 && desktop != NETWinInfo::OnAllDesktops )
        desktop = KWin::currentDesktop();

    // show the note if desired
    if ( desktop != 0 && !isVisible() )
    {
        // HACK HACK
        if ( desktop != NETWinInfo::OnAllDesktops )
        {
            // to avoid flicker, call this before show()
            toDesktop( desktop );
            show();
        }
        else
        {
            show();
            // if this is called before show(),
            // it won't work for sticky notes!!!
            toDesktop( desktop );
        }
    }

    m_editor->setText( m_journal->description() );
    m_editor->setModified( false );
}

KNote::~KNote()
{
    delete m_config;
}


// -------------------- public slots -------------------- //

void KNote::slotKill( bool force )
{
    if ( !force &&
         KMessageBox::warningYesNo( this,
            i18n("<qt>Do you really want to delete note <b>%1</b>?</qt>")
                .arg( m_label->text() ),
            i18n("Confirm Delete") )
         != KMessageBox::Yes )
    {
        return;
    }

    // delete the configuration first, then the corresponding file
    delete m_config;
    m_config = 0;

    QString configFile = KGlobal::dirs()->saveLocation( "appdata", "notes/" );
    configFile += m_journal->uid();

    if ( !KIO::NetAccess::del( KURL(configFile), this ) )
        kdError(5500) << "Can't remove the note config: " << configFile << endl;

    emit sigKillNote( m_journal );
}


// -------------------- public member functions -------------------- //

void KNote::saveData()
{
    m_journal->setSummary( m_label->text() );
    m_journal->setDescription( m_editor->text() );

    emit sigDataChanged();
    m_editor->setModified( false );
}

void KNote::saveConfig() const
{
    m_config->setWidth( width() );
    m_config->setHeight( height() - (m_tool->isHidden() ? 0 : m_tool->height()) );
    m_config->setPosition( pos() );

    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop | NET::WMState );
    m_config->setDesktop( wm_client.desktop() );

    // actually store the config on disk
    m_config->writeConfig();
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
    updateLabelAlignment();

    if ( m_editor )    // not called from CTOR?
        saveData();

    // set the window's name for the taskbar entry to be more helpful (#58338)
    NETWinInfo note_win( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop );
    note_win.setName( name.utf8() );

    emit sigNameChanged();
}

void KNote::setText( const QString& text )
{
    m_editor->setText( text );
    saveData();
}

// FIXME KDE 4.0: remove sync(), isNew() and isModified()
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

    // hacky... not possible with KConfig XT
    KConfig *config = m_config->config();
    config->setGroup( "Synchronisation" );
    config->writeEntry( app, result.data() );
}

bool KNote::isNew( const QString& app ) const
{
    KConfig *config = m_config->config();
    config->setGroup( "Synchronisation" );
    QString hash = config->readEntry( app );
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

    KConfig *config = m_config->config();
    config->setGroup( "Synchronisation" );
    QString orig = config->readEntry( app );

    if ( hash.verify( orig.utf8() ) )   // returns false on error!
        return false;
    else
        return true;
}

void KNote::toDesktop( int desktop )
{
    if ( desktop == 0 || desktop == NETWinInfo::OnAllDesktops )
        KWin::setOnAllDesktops( winId(), true );
    else
        KWin::setOnDesktop( winId(), desktop );
}


// ------------------ private slots (menu actions) ------------------ //

void KNote::slotRename()
{
    // pop up dialog to get the new name
    bool ok;
    QString newName = KInputDialog::getText( QString::null,
        i18n("Please enter the new name:"), m_label->text(), &ok, this );
    if ( !ok ) // handle cancel
        return;

    setName( newName );
}

void KNote::slotClose()
{
    m_editor->clearFocus();
    hide(); //just hide the note so it's still available from the dock window
}

void KNote::slotInsDate()
{
    m_editor->insert( KGlobal::locale()->formatDateTime(QDateTime::currentDateTime()) );
}

void KNote::slotPreferences()
{
    // reuse if possible
    if ( KNoteConfigDlg::showDialog( noteId().utf8() ) )
        return;

    // create a new preferences dialog...
    KNoteConfigDlg *dialog = new KNoteConfigDlg( m_config, name(), false, this,
                                                 noteId().utf8() );
    connect( dialog, SIGNAL(settingsChanged()), this, SLOT(slotApplyConfig()) );
    connect( this, SIGNAL(sigNameChanged()), dialog, SLOT(slotUpdateCaption()) );
    dialog->show();
}

void KNote::slotSend()
{
    // pop up dialog to get the IP
    KNoteHostDlg hostDlg( i18n("Send \"%1\"").arg( name() ), this );
    bool ok = (hostDlg.exec() == QDialog::Accepted);
    QString host = hostDlg.host();

    if ( !ok ) // handle cancel
        return;

    if ( host.isEmpty() )
    {
        KMessageBox::sorry( this, i18n("The host cannot be empty.") );
        return;
    }

    // Send the note
    (void)new KNotesNetworkSender( host, text() );
}

void KNote::slotMail()
{
    saveData();

    QString msg_body = m_editor->text();

    // convert rich text to plain text first
    if ( m_editor->textFormat() == RichText )
    {
        QTextEdit conv;
        conv.setTextFormat( RichText );
        conv.setText( msg_body );
        conv.setTextFormat( PlainText );
        msg_body = conv.text();
    }

    // get the mail action command
    QStringList cmd_list = QStringList::split( QChar(' '), KNotesGlobalConfig::mailAction() );

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
        KMessageBox::sorry( this, i18n("Unable to start the mail process.") );
}

void KNote::slotPrint()
{
    saveData();

    KPrinter printer;
    printer.setFullPage( true );

    if ( printer.setup(0L, i18n("Print %1").arg(name())) )
    {
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

        QSimpleRichText text( content, m_config->font(), m_editor->context(),
                              m_editor->styleSheet(), m_editor->mimeSourceFactory(),
                              body.height() /*, linkColor, linkUnderline? */ );

        text.setWidth( &painter, body.width() );
        QRect view( body );

        int page = 1;

        for (;;)
        {
            text.draw( &painter, body.left(), body.top(), view, colorGroup() );
            view.moveBy( 0, body.height() );
            painter.translate( 0, -body.height() );

            // page numbers
            painter.setFont( m_config->font() );
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

void KNote::slotPopupActionToDesktop( int id )
{
    if( id > 1 )
      --id;      // compensate for the menu separator
    toDesktop( id );
}


// ------------------ private slots (configuration) ------------------ //

void KNote::slotApplyConfig()
{
    if ( m_config->richText() )
        m_editor->setTextFormat( RichText );
    else
        m_editor->setTextFormat( PlainText );

    m_label->setFont( m_config->titleFont() );
    m_editor->setTextFont( m_config->font() );
    m_editor->setTabStop( m_config->tabSize() );
    m_editor->setAutoIndentMode( m_config->autoIndent() );

    // if called as a slot, save the text, we might have changed the
    // text format - otherwise the journal will not be updated
    if ( sender() )
        saveData();

    setColor( m_config->fgColor(), m_config->bgColor() );

    updateLabelAlignment();
    slotUpdateShowInTaskbar();
}

void KNote::slotUpdateKeepAboveBelow()
{
    KWin::WindowInfo info( KWin::windowInfo( winId() ) );

    if ( m_keepAbove->isChecked() )
    {
        m_config->setKeepAbove( true );
        m_config->setKeepBelow( false );
        KWin::setState( winId(), info.state() | NET::KeepAbove );
    }
    else if ( m_keepBelow->isChecked() )
    {
        m_config->setKeepAbove( false );
        m_config->setKeepBelow( true );
        KWin::setState( winId(), info.state() | NET::KeepBelow );
    }
    else
    {
        m_config->setKeepAbove( false );
        KWin::clearState( winId(), NET::KeepAbove );

        m_config->setKeepBelow( false );
        KWin::clearState( winId(), NET::KeepBelow );
    }
}

void KNote::slotUpdateShowInTaskbar()
{
    if ( !m_config->showInTaskbar() )
        KWin::setState( winId(), KWin::windowInfo(winId()).state() | NET::SkipTaskbar );
    else
        KWin::clearState( winId(), NET::SkipTaskbar );
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

    if ( wm_client.desktop() == NETWinInfo::OnAllDesktops )
        m_toDesktop->setCurrentItem( 0 );
    else
        m_toDesktop->setCurrentItem( wm_client.desktop() + 1 ); // compensate for separator (+1)
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
    newpalette.setColor( QColorGroup::Shadow, bg.dark(116) );  // 132 ?
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
    emit sigColorChanged();
}

void KNote::updateLabelAlignment()
{
    // if the name is too long to fit, left-align it, otherwise center it (#59028)
    QString labelText = m_label->text();
    if ( m_label->fontMetrics().boundingRect( labelText ).width() > m_label->width() )
        m_label->setAlignment( AlignLeft );
    else
        m_label->setAlignment( AlignHCenter );
}

void KNote::updateFocus()
{
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
            setGeometry( x(), y(), width(), height() - m_tool->height() );
            updateLayout();     // to update the minimum height
        }
    }
}

void KNote::updateLayout()
{
    // DAMN, Qt still has no support for widgets with a fixed aspect ratio :-(
    // So we have to write our own layout manager...

    const int headerHeight = m_label->sizeHint().height();
    const int toolHeight = m_tool->isHidden() ? 0 : 16;
    const int margin = m_editor->margin();
    static const int border = 2;
    bool closeLeft = false;

//    if ( KDecoration::options()->customButtonPositions() )
//       closeLeft = KDecoration::options()->titleButtonsLeft().find( 'X' ) > -1;

    m_button->setGeometry(
        closeLeft ? frameRect().x() + border
                  : frameRect().width() - headerHeight - border,
        frameRect().y() + border,
        headerHeight,
        headerHeight
    );

    m_label->setGeometry(
        frameRect().x() + border + (closeLeft && !m_button->isHidden() ? headerHeight : 0),
        frameRect().y() + border,
        frameRect().width() - (m_button->isHidden() ? 0 : headerHeight) - border*2,
        headerHeight
    );

    m_editor->setGeometry(
        contentsRect().x(),
        contentsRect().y() + headerHeight + border,
        contentsRect().width(),
        contentsRect().height() - headerHeight - toolHeight - border*2
    );

    m_tool->setGeometry(
        contentsRect().x(),
        contentsRect().height() - 16,
        contentsRect().width(),
        16
    );
    m_tool->setIconSize( 10 );

    // if there was just a way of making KComboBox adhere the toolbar height...
    QObjectList *list = m_tool->queryList( "KComboBox" );
    QObjectListIt it( *list );
    while ( it.current() != 0 && toolHeight )
    {
        KComboBox *combo = (KComboBox *)it.current();
        QFont font = combo->font();
        font.setPointSize( 7 );
        combo->setFont( font );
        combo->setFixedHeight( m_tool->height() - 2 );
        ++it;
    }
    delete list;

    setMinimumSize(
        m_editor->cornerWidget()->width() + margin*2 + border*2,
        headerHeight + toolHeight +
                m_editor->cornerWidget()->height() + margin*2 + border*2
    );

    updateLabelAlignment();
}

// -------------------- protected methods -------------------- //

void KNote::showEvent( QShowEvent * )
{
    // KWin does not preserve these properties for hidden windows
    slotUpdateKeepAboveBelow();
    slotUpdateShowInTaskbar();
}

void KNote::resizeEvent( QResizeEvent *qre )
{
    QFrame::resizeEvent( qre );
    updateLayout();
}

void KNote::closeEvent( QCloseEvent * )
{
    slotClose();
}

void KNote::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_Escape )
        slotClose();
    else
        e->ignore();
}

bool KNote::focusNextPrevChild( bool )
{
    return true;
}

bool KNote::event( QEvent *ev )
{
    if ( ev->type() == QEvent::LayoutHint )
    {
        updateLayout();
        return true;
    }
    else
        return QFrame::event( ev );
}

bool KNote::eventFilter( QObject *o, QEvent *ev )
{
    if ( o == m_label )
    {
        QMouseEvent *e = (QMouseEvent *)ev;

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

            e->button() == LeftButton ? KWin::raiseWindow( winId() )
                                      : KWin::lowerWindow( winId() );

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
            QFocusEvent *fe = static_cast<QFocusEvent *>(ev);
            if ( fe->reason() != QFocusEvent::Popup && fe->reason() != QFocusEvent::Mouse )
            {
                updateFocus();
                if ( m_editor->isModified() )
                    saveData();
            }
        }
        else if ( ev->type() == QEvent::FocusIn )
            updateFocus();

        return false;
    }

    if ( o == m_editor->viewport() )
    {
        if ( m_edit_menu &&
             ev->type() == QEvent::MouseButtonPress &&
             ((QMouseEvent *)ev)->button() == RightButton )
        {
            m_edit_menu->popup( QCursor::pos() );
            return true;
        }
    }

    return false;
}

#include "knote.moc"
#include "knotebutton.moc"
