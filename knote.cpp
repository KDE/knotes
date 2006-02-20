/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2006, The KNotes Developers

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*******************************************************************/

#include <QLabel>
#include <QSize>
#include <QSizeGrip>
#include <QScrollBar>
#include <QBitmap>
#include <QCursor>
#include <QPainter>
#include <QObject>
#include <QFile>
#include <QCheckBox>
#include <QTextStream>
#include <QDesktopWidget>
#include <QPixmap>

#include <kapplication.h>
#include <kdebug.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kcombobox.h>
#include <ktoolbar.h>
#include <kmenu.h>
#include <kxmlguibuilder.h>
#include <kxmlguifactory.h>
#include <k3colordrag.h>
#include <kiconeffect.h>
#include <kprinter.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kfind.h>
#include <kprocess.h>
#include <kinputdialog.h>
#include <kcodecs.h>
#include <kglobalsettings.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>

#include <libkcal/journal.h>

#include "knote.h"
#include "knotebutton.h"
#include "knoteedit.h"
#include "knoteconfig.h"
#include "knotesglobalconfig.h"
#include "knoteconfigdlg.h"
#include "knotealarmdlg.h"
#include "knotehostdlg.h"
#include "knotesnetsend.h"
#include "version.h"

#include <kwin.h>
#include <netwm.h>

#include <fixx11h.h>
#include <QX11Info>
#include <kiconloader.h>

using namespace KCal;


KNote::KNote( QDomDocument buildDoc, Journal *j, QWidget *parent )
  : QFrame( parent, Qt::FramelessWindowHint ),
    m_label( 0 ), m_grip( 0 ), m_button( 0 ),
    m_tool( 0 ), m_editor( 0 ), m_config( 0 ), m_journal( j ), m_find( 0 ),
    m_kwinConf( KSharedConfig::openConfig( "kwinrc", true ) )
{
    setAttribute( Qt::WA_DeleteOnClose );
    setAcceptDrops( true );
    setObjectName( m_journal->uid() );

    actionCollection()->setWidget( this );

    setDOMDocument( buildDoc );

    // just set the name of the file to save the actions to, do NOT reparse it
    setXMLFile( instance()->instanceName() + "ui.rc", false, false );

    // if there is no title yet, use the start date if valid
    // (KOrganizer's journals don't have titles but a valid start date)
    if ( m_journal->summary().isNull() && m_journal->dtStart().isValid() )
    {
        QString s = KGlobal::locale()->formatDateTime( m_journal->dtStart() );
        m_journal->setSummary( s );
    }

    // create the menu items for the note - not the editor...
    // rename, mail, print, save as, insert date, alarm, close, delete, new note
    new KAction( i18n("New"), "filenew", 0,
        this, SIGNAL(sigRequestNewNote()), actionCollection(), "new_note" );
    new KAction( i18n("Rename..."), "text", 0,
        this, SLOT(slotRename()), actionCollection(), "rename_note" );
    m_readOnly = new KToggleAction( i18n("Lock"), "lock" , 0,
        this, SLOT(slotUpdateReadOnly()), actionCollection(), "lock_note" );
    m_readOnly->setCheckedState( KGuiItem( i18n("Unlock"), "unlock" ) );
    new KAction( i18n("Hide"), "fileclose" , Qt::Key_Escape,
        this, SLOT(slotClose()), actionCollection(), "hide_note" );
    new KAction( i18n("Delete"), "knotes_delete", 0,
        this, SLOT(slotKill()), actionCollection(), "delete_note" );

    new KAction( i18n("Insert Date"), "knotes_date", 0 ,
        this, SLOT(slotInsDate()), actionCollection(), "insert_date" );
    new KAction( i18n("Set Alarm..."), "knotes_alarm", 0 ,
        this, SLOT(slotSetAlarm()), actionCollection(), "set_alarm" );

    new KAction( i18n("Send..."), "network", 0,
        this, SLOT(slotSend()), actionCollection(), "send_note" );
    new KAction( i18n("Mail..."), "mail_send", 0,
        this, SLOT(slotMail()), actionCollection(), "mail_note" );
    new KAction( i18n("Save As..."), "filesaveas", 0,
        this, SLOT(slotSaveAs()), actionCollection(), "save_note" );
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

    // invisible action to walk through the notes to make this configurable
//FIXME: this backtab thing doesn't work anymore!
    new KAction( i18n("Walk Through Notes"), 0, Qt::SHIFT+Qt::Key_Backtab,
                 this, SIGNAL(sigShowNextNote()), actionCollection(), "walk_notes" );

    // create the note header, button and label...
    m_label = new QLabel( this );
    m_label->setFrameStyle( NoFrame );
    m_label->setBackgroundRole( QPalette::Base );
    m_label->setLineWidth( 0 );
    m_label->setAutoFillBackground( true );
    m_label->installEventFilter( this );  // receive events (for dragging & action menu)
    setName( m_journal->summary() );      // don't worry, no signals are connected at this stage yet

    m_button = new KNoteButton( "knotes_close", this );
    connect( m_button, SIGNAL(clicked()), this, SLOT(slotClose()) );

    // create the note editor
    m_editor = new KNoteEdit( actionCollection(), this );
    m_editor->installEventFilter( this ); // receive focus events for modified

    // now that we have created all actions build the gui
    KXMLGUIBuilder builder( this );
    KXMLGUIFactory factory( &builder, this );
    factory.addClient( this );

    m_menu = static_cast<KMenu*>(factory.container( "note_context", this ));
    m_editor->setContextMenu( static_cast<KMenu*>(factory.container( "note_edit", this )) ); 

    m_tool = static_cast<KToolBar*>(factory.container( "note_tool", this ));
    m_tool->setIconSize( 10 );
    m_tool->setFixedHeight( 16 );
    m_tool->setIconText( KToolBar::IconOnly );

    // if there was just a way of making KComboBox adhere the toolbar height...
    foreach ( KComboBox *combo, m_tool->findChildren<KComboBox *>() )
    {
        QFont font = combo->font();
        font.setPointSize( 7 );
        combo->setFont( font );
        combo->setFixedHeight( 14 );
    }
    m_tool->hide();

    setFocusProxy( m_editor );

    // create the resize handle
    m_grip = new QSizeGrip( this );
    uint width = m_editor->verticalScrollBar()->sizeHint().width();
    uint height = m_editor->horizontalScrollBar()->sizeHint().height();
    QBitmap mask( width, height );
    mask.fill( Qt::color0 );
    QPolygon array;
    array.setPoints( 3, 0, height, width, height, width, 0 );
    QPainter p;
    p.begin( &mask );
    p.setBrush( Qt::color1 );
    p.drawPolygon( array );
    p.end();
    m_grip->setMask( mask );
    m_grip->setBackgroundRole( QPalette::Base );

    // the config file location
    QString configFile = KGlobal::dirs()->saveLocation( "appdata", "notes/" );
    configFile += m_journal->uid();

    // no config file yet? -> use the default display config if available
    // we want to write to configFile, so use "false"
    bool newNote = !KIO::NetAccess::exists( KUrl::fromPathOrURL( configFile ), false, 0 );

    m_config = new KNoteConfig( KSharedConfig::openConfig( configFile, false, false ) );
    m_config->readConfig();
    m_config->setVersion( KNOTES_VERSION );

    if ( newNote )
    {
        // until kdelibs provides copying of KConfigSkeletons (KDE 3.4)
        KNotesGlobalConfig *globalConfig = KNotesGlobalConfig::self();
        m_config->setBgColor( globalConfig->bgColor() );
        m_config->setFgColor( globalConfig->fgColor() );
        m_config->setWidth( globalConfig->width() );
        m_config->setHeight( globalConfig->height() );

        m_config->setFont( globalConfig->font() );
        m_config->setTitleFont( globalConfig->titleFont() );
        m_config->setAutoIndent( globalConfig->autoIndent() );
        m_config->setRichText( globalConfig->richText() );
        m_config->setTabSize( globalConfig->tabSize() );
        m_config->setReadOnly( globalConfig->readOnly() );

        m_config->setDesktop( globalConfig->desktop() );
        m_config->setHideNote( globalConfig->hideNote() );
        m_config->setPosition( globalConfig->position() );
        m_config->setShowInTaskbar( globalConfig->showInTaskbar() );
        m_config->setKeepAbove( globalConfig->keepAbove() );
        m_config->setKeepBelow( globalConfig->keepBelow() );

        m_config->writeConfig();
    }

    // set up the look&feel of the note
    setFrameStyle( Panel | Raised );
    setMinimumSize( 20, 20 );
    setBackgroundRole( QPalette::Base );

    m_editor->setContentsMargins( 0, 0, 0, 0 );
    m_editor->setBackgroundRole( QPalette::Base );
    m_editor->setFrameStyle( NoFrame );
    m_editor->setText( m_journal->description() );
    m_editor->document()->setModified( false );

    // can be done here since this doesn't pick up changes while KNotes is running anyway
    bool closeLeft = false;
    m_kwinConf->setGroup( "Style" );
    if ( m_kwinConf->readEntry( "CustomButtonPositions",false ) )
        closeLeft = m_kwinConf->readEntry( "ButtonsOnLeft").contains( 'X' );

    // load the display configuration of the note
    width = m_config->width();
    height = m_config->height();
    resize( width, height );

    // let KWin do the placement if the position is illegal--at least 10 pixels
    // of a note need to be visible
    const QPoint& position = m_config->position();
    QRect desk = kapp->desktop()->rect();
    desk.adjust( 10, 10, -10, -10 );
    if ( desk.intersects( QRect( position, QSize( width, height ) ) ) )
        move( position );           // do before calling show() to avoid flicker

    // config items in the journal have priority
    QString property = m_journal->customProperty( "KNotes", "FgColor" );
    if ( !property.isNull() )
        m_config->setFgColor( QColor( property ) );
    else
        m_journal->setCustomProperty( "KNotes", "FgColor", m_config->fgColor().name() );

    property = m_journal->customProperty( "KNotes", "BgColor" );
    if ( !property.isNull() )
        m_config->setBgColor( QColor( property ) );
    else
        m_journal->setCustomProperty( "KNotes", "BgColor", m_config->bgColor().name() );

    property = m_journal->customProperty( "KNotes", "RichText" );
    if ( !property.isNull() )
        m_config->setRichText( property == "true" ? true : false );
    else
        m_journal->setCustomProperty( "KNotes", "RichText", m_config->richText() ? "true" : "false" );

    // read configuration settings...
    slotApplyConfig();

    // if this is a new note put on current desktop - we can't use defaults
    // in KConfig XT since only _changes_ will be stored in the config file
    int desktop = m_config->desktop();
    if ( desktop < 0 && desktop != NETWinInfo::OnAllDesktops )
        desktop = KWin::currentDesktop();

    // show the note if desired
    if ( desktop != 0 && !m_config->hideNote() )
    {
        // to avoid flicker, call this before show()
        toDesktop( desktop );
        show();

        // because KWin forgets about that for hidden windows
        if ( desktop == NETWinInfo::OnAllDesktops )
            toDesktop( desktop );
    }

    m_readOnly->setChecked( m_config->readOnly() );
    slotUpdateReadOnly();

    if ( m_config->keepAbove() )
        m_keepAbove->setChecked( true );
    else if ( m_config->keepBelow() )
        m_keepBelow->setChecked( true );
    else
    {
        m_keepAbove->setChecked( false );
        m_keepBelow->setChecked( false );
    }
    slotUpdateKeepAboveBelow();

    // HACK: update the icon color - again after showing the note, to make kicker aware of the new colors
    KIconEffect effect;
    QPixmap icon = effect.apply( qApp->windowIcon().pixmap( IconSize(KIcon::Desktop), IconSize(KIcon::Desktop) ),
                                 KIconEffect::Colorize, 1, m_config->bgColor(), false );
    QPixmap miniIcon = effect.apply( qApp->windowIcon().pixmap( IconSize(KIcon::Small), IconSize(KIcon::Small) ),
                                     KIconEffect::Colorize, 1, m_config->bgColor(), false );
    KWin::setIcons( winId(), icon, miniIcon );
}

KNote::~KNote()
{
    delete m_config;
}


// -------------------- public slots -------------------- //

void KNote::slotKill( bool force )
{
    if ( !force &&
         KMessageBox::warningContinueCancel( this,
             i18n("<qt>Do you really want to delete note <b>%1</b>?</qt>").arg( m_label->text() ),
             i18n("Confirm Delete"), KGuiItem( i18n("&Delete"), "editdelete" ),
             "ConfirmDeleteNote"
         )
         != KMessageBox::Continue )
    {
        return;
    }

    // delete the configuration first, then the corresponding file
    delete m_config;
    m_config = 0;

    QString configFile = KGlobal::dirs()->saveLocation( "appdata", "notes/" );
    configFile += m_journal->uid();

    if ( !KIO::NetAccess::del( KUrl::fromPathOrURL( configFile ), this ) )
        kError(5500) << "Can't remove the note config: " << configFile << endl;

    emit sigKillNote( m_journal );
}


// -------------------- public member functions -------------------- //

void KNote::saveData()
{
    m_journal->setSummary( m_label->text() );
    m_journal->setDescription( m_editor->text() );
    m_journal->setCustomProperty( "KNotes", "FgColor", m_config->fgColor().name() );
    m_journal->setCustomProperty( "KNotes", "BgColor", m_config->bgColor().name() );
    m_journal->setCustomProperty( "KNotes", "RichText", m_config->richText() ? "true" : "false" );

    emit sigDataChanged();
    m_editor->document()->setModified( false );
}

void KNote::saveConfig() const
{
    m_config->setWidth( width() );
    m_config->setHeight( height() - (m_tool->isHidden() ? 0 : m_tool->height()) );
    m_config->setPosition( pos() );

    NETWinInfo wm_client( QX11Info::display(), winId(), QX11Info::appRootWindow(), NET::WMDesktop );
    if ( wm_client.desktop() == NETWinInfo::OnAllDesktops || wm_client.desktop() > 0 )
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
    NETWinInfo note_win( QX11Info::display(), winId(), QX11Info::appRootWindow(), NET::WMDesktop );
    note_win.setName( name.toUtf8() );

    emit sigNameChanged();
}

void KNote::setText( const QString& text )
{
    m_editor->setText( text );
    saveData();
}

void KNote::find( const QString& pattern, long options )
{
    delete m_find;
    m_find = new KFind( pattern, options, this );

    connect( m_find, SIGNAL(highlight( const QString &, int, int )),
             this, SLOT(slotHighlight( const QString &, int, int )) );
    connect( m_find, SIGNAL(findNext()), this, SLOT(slotFindNext()) );

    m_find->setData( m_editor->toPlainText() );
    slotFindNext();
}

void KNote::slotFindNext()
{
    // TODO: honor FindBackwards
    // TODO: dialogClosed() -> delete m_find

    // Let KFind inspect the text fragment, and display a dialog if a match is found
    KFind::Result res = m_find->find();

    if ( res == KFind::NoMatch ) // i.e. at end-pos
    {
        // use a different text cursor!
        m_editor->textCursor().clearSelection();
        emit sigFindFinished();
        delete m_find;
        m_find = 0;
    }
    else
    {
        show();
        KWin::setCurrentDesktop( KWin::windowInfo( winId() ).desktop() );
    }
}

void KNote::slotHighlight( const QString& /*str*/, int idx, int len )
{
    QTextCursor c = m_editor->textCursor();
    c.setPosition( idx );
    c.setPosition( idx + len, QTextCursor::KeepAnchor );
    // TODO: modify the selection color, use a different QTextCursor?
}

bool KNote::isModified() const
{
    return m_editor->document()->isModified();
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

void KNote::slotUpdateReadOnly()
{
    const bool readOnly = m_readOnly->isChecked();

    m_editor->setReadOnly( readOnly );
    m_config->setReadOnly( readOnly );

    // enable/disable actions accordingly
    actionCollection()->action( "configure_note" )->setEnabled( !readOnly );
    actionCollection()->action( "insert_date" )->setEnabled( !readOnly );
    actionCollection()->action( "delete_note" )->setEnabled( !readOnly );

    actionCollection()->action( "edit_undo" )->setEnabled( !readOnly && m_editor->document()->isUndoAvailable() );
    actionCollection()->action( "edit_redo" )->setEnabled( !readOnly && m_editor->document()->isRedoAvailable() );
    actionCollection()->action( "edit_cut" )->setEnabled( !readOnly && m_editor->textCursor().hasSelection() );
    actionCollection()->action( "edit_paste" )->setEnabled( !readOnly );
    actionCollection()->action( "edit_clear" )->setEnabled( !readOnly );

    updateFocus();
}

void KNote::slotClose()
{
    NETWinInfo wm_client( QX11Info::display(), winId(), QX11Info::appRootWindow(), NET::WMDesktop );
    if ( wm_client.desktop() == NETWinInfo::OnAllDesktops || wm_client.desktop() > 0 )
        m_config->setDesktop( wm_client.desktop() );

    m_editor->clearFocus();
    m_config->setHideNote( true );
    m_config->setPosition( pos() );

    // just hide the note so it's still available from the dock window
    hide();
}

void KNote::slotInsDate()
{
    m_editor->insertPlainText( KGlobal::locale()->formatDateTime(QDateTime::currentDateTime()) );
}

void KNote::slotSetAlarm()
{
    KNoteAlarmDlg dlg( name(), this );
    dlg.setIncidence( m_journal );

    if ( dlg.exec() == QDialog::Accepted )
        emit sigDataChanged();
}

void KNote::slotPreferences()
{
    // reuse if possible
    if ( KNoteConfigDlg::showDialog( noteId() ) )
        return;

    // create a new preferences dialog...
    KNoteConfigDlg *dialog = new KNoteConfigDlg( m_config, name(), this, noteId() );
    connect( dialog, SIGNAL(settingsChanged(const QString&)), this, SLOT(slotApplyConfig()) );
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
#warning Port network sending!
#if 0
    KNotesNetworkSender *sender = new KNotesNetworkSender( host, KNotesGlobalConfig::port() );
    sender->setSenderId( KNotesGlobalConfig::senderID() );
    sender->setNote( name(), text() ); // FIXME: plainText ??
    sender->connect();
#endif
}

void KNote::slotMail()
{
    // get the mail action command
    QStringList cmd_list = KNotesGlobalConfig::mailAction().split( QChar(' '), QString::SkipEmptyParts );

    KProcess mail;
    foreach ( QString cmd, cmd_list )
    {
        if ( cmd == "%f" )
            mail << m_editor->toPlainText();
        else if ( cmd == "%t" )
            mail << m_label->text();
        else
            mail << cmd;
    }

    if ( !mail.start( KProcess::DontCare ) )
        KMessageBox::sorry( this, i18n("Unable to start the mail process.") );
}

void KNote::slotPrint()
{
    saveData();
#warning Port printing!
#if 0
    KPrinter printer;
    printer.setFullPage( true );

    if ( printer.setup( 0, i18n("Print %1").arg(name()) ) )
    {
        QPainter painter;
        painter.begin( &printer );

        const int margin = 40;  // pt

        int marginX = margin * painter.device()->logicalDpiX() / 72;
        int marginY = margin * painter.device()->logicalDpiY() / 72;

        QRect body( marginX, marginY,
                    painter.device()->width() - marginX * 2,
                    painter.device()->height() - marginY * 2 );

        QString content = m_editor->toHtml();

        Q3SimpleRichText text( content, m_config->font(), m_editor->context(),
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
#endif
}

void KNote::slotSaveAs()
{
    // TODO: where to put pdf file support? In the printer??!??!

    QCheckBox *convert = 0;

    if ( m_editor->acceptRichText() )
    {
        convert = new QCheckBox( 0 );
        convert->setText( i18n("Save note as plain text") );
    }

    KFileDialog dlg( QString::null, QString::null, this, convert );
    dlg.setOperationMode( KFileDialog::Saving );
    dlg.setCaption( i18n("Save As") );
    dlg.exec();

    QString fileName = dlg.selectedFile();
    if ( fileName.isEmpty() )
        return;

    QFile file( fileName );

    if ( file.exists() &&
         KMessageBox::warningContinueCancel( this, i18n("<qt>A file named <b>%1</b> already exists.<br>"
                           "Are you sure you want to overwrite it?</qt>").arg( QFileInfo(file).fileName() ) )
         != KMessageBox::Continue )
    {
        return;
    }

    if ( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &file );
        if ( convert && !convert->isChecked() )
            stream << m_editor->toHtml();
        else
            stream << m_editor->toPlainText();
    }
}

void KNote::slotPopupActionToDesktop( int id )
{
    toDesktop( id - 1 ); // compensate for the menu separator, -1 == all desktops
}


// ------------------ private slots (configuration) ------------------ //

void KNote::slotApplyConfig()
{
    m_label->setFont( m_config->titleFont() );
    m_editor->setRichText( m_config->richText() );
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
    NETRootInfo wm_root( QX11Info::display(), NET::NumberOfDesktops | NET::DesktopNames );
    NETWinInfo wm_client( QX11Info::display(), winId(), QX11Info::appRootWindow(), NET::WMDesktop );

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

void KNote::toDesktop( int desktop )
{
    if ( desktop == 0 )
        return;

    if ( desktop == NETWinInfo::OnAllDesktops )
        KWin::setOnAllDesktops( winId(), true );
    else
        KWin::setOnDesktop( winId(), desktop );
}

void KNote::setColor( const QColor &fg, const QColor &bg )
{
    QPalette p = palette();

    // better: from light(150) to light(100) to light(75)
    //QLinearGradient g( width()/2, 0, width()/2, height() );
    //g.setColorAt( 0, bg );
    //g.setColorAt( 1, bg.dark(150) );

    p.setColor( QPalette::Window,     bg );
    //p.setBrush( QPalette::Window,     g );
    p.setColor( QPalette::Base,       bg );
    //p.setBrush( QPalette::Base,       g );

    p.setColor( QPalette::WindowText, fg );
    p.setColor( QPalette::Text,       fg );

    p.setColor( QPalette::Button,     bg.dark(116) );
    p.setColor( QPalette::ButtonText, fg );

    //p.setColor( QColorGroup::Highlight,  bg );
    //p.setColor( QColorGroup::HighlightedText, fg );

    // order: Light, Midlight, Button, Mid, Dark, Shadow
    
    // the shadow
    p.setColor( QPalette::Light, bg.light(180) );
    p.setColor( QPalette::Midlight, bg.light(150) );
    p.setColor( QPalette::Mid, bg.light(150) );
    p.setColor( QPalette::Dark, bg.dark(108) );
    p.setColor( QPalette::Shadow, bg.dark(116) );

    setPalette( p );

    // darker values for the active label
    p.setColor( QPalette::Active, QPalette::Base, bg.dark(116) );

    m_label->setPalette( p );
    
    // set the text color
    m_editor->setTextColor( fg );

    
    // update the icon color
    KIconEffect effect;
    QPixmap icon = effect.apply( qApp->windowIcon().pixmap( IconSize(KIcon::Desktop),
                                                            IconSize(KIcon::Desktop) ),
                                 KIconEffect::Colorize, 1, bg, false );
    QPixmap miniIcon = effect.apply( qApp->windowIcon().pixmap( IconSize(KIcon::Small), 
                                                                IconSize(KIcon::Small) ),
                                     KIconEffect::Colorize, 1, bg, false );
    KWin::setIcons( winId(), icon, miniIcon );

    // update the color of the title
    updateFocus();
    emit sigColorChanged();
}

void KNote::updateLabelAlignment()
{
    // if the name is too long to fit, left-align it, otherwise center it (#59028)
    QString labelText = m_label->text();
    if ( m_label->fontMetrics().boundingRect( labelText ).width() > m_label->width() )
        m_label->setAlignment( Qt::AlignLeft );
    else
        m_label->setAlignment( Qt::AlignHCenter );
}

void KNote::updateFocus()
{
    if ( hasFocus() )
    {
        m_button->show();
        m_grip->show();

        if ( !m_editor->isReadOnly() )
        {
            if ( m_tool->isHidden() && m_editor->acceptRichText() )
            {
                m_tool->show();
                setGeometry( x(), y(), width(), height() + m_tool->height() );
            }
        }
        else if ( !m_tool->isHidden() )
        {
            m_tool->hide();
            setGeometry( x(), y(), width(), height() - m_tool->height() );
            updateLayout();     // to update the minimum height
        }
    }
    else
    {
        m_button->hide();
        m_grip->hide();

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
    const int headerHeight = m_label->sizeHint().height();
    const int margin = 0; // FIXME  m_editor->margin();
    bool closeLeft = false;

    m_kwinConf->setGroup( "Style" );
    if ( m_kwinConf->readEntry( "CustomButtonPositions",false ) )
        closeLeft = m_kwinConf->readEntry( "ButtonsOnLeft" ).contains( 'X' );

    m_button->setGeometry(
        closeLeft ? contentsRect().x() : contentsRect().width() - headerHeight,
        contentsRect().y(),
        headerHeight,
        headerHeight
    );

    m_label->setGeometry(
        contentsRect().x(), contentsRect().y(),
        contentsRect().width(), headerHeight
    );

    m_editor->setGeometry( QRect(
        QPoint( contentsRect().x(),
                contentsRect().y() + headerHeight ),
        QPoint( contentsRect().right(),
                contentsRect().bottom() - (m_tool->isHidden() ? 0 : m_tool->height()) )
    ) );

    m_tool->setGeometry(
        contentsRect().x(),
        contentsRect().bottom() - m_tool->height() + 1,
        contentsRect().width(),
        m_tool->height()
    );

    setMinimumSize(
        /* FIXME m_editor->cornerWidget()->width() + */ margin*2,
        headerHeight + (m_tool->isHidden() ? 0 : m_tool->height()) +
                /* m_editor->cornerWidget()->height() + */ margin*2
    );

    updateLabelAlignment();
}

// -------------------- protected methods -------------------- //

void KNote::contextMenuEvent( QContextMenuEvent *e )
{
    if ( m_menu )
        m_menu->popup( e->globalPos() );
}

void KNote::showEvent( QShowEvent * )
{
    if ( m_config->hideNote() )
    {
        // KWin does not preserve these properties for hidden windows
        slotUpdateKeepAboveBelow();
        slotUpdateShowInTaskbar();
        toDesktop( m_config->desktop() );
        move( m_config->position() );
        m_config->setHideNote( false );
    }
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

void KNote::dragEnterEvent( QDragEnterEvent *e )
{
    if ( !m_config->readOnly() )
        e->setAccepted( K3ColorDrag::canDecode( e ) );
}

void KNote::dropEvent( QDropEvent *e )
{
    if ( m_config->readOnly() )
        return;

    QColor bg;
    if ( K3ColorDrag::decode( e, bg ) )
    {
        setColor( palette().color( foregroundRole() ), bg );
        m_journal->setCustomProperty( "KNotes", "BgColor", bg.name() );
        m_config->setBgColor( bg );
    }
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
    if ( ev->type() == QEvent::DragEnter &&
         K3ColorDrag::canDecode( static_cast<QDragEnterEvent *>(ev) ) )
    {
        dragEnterEvent( static_cast<QDragEnterEvent *>(ev) );
        return true;
    }

    if ( ev->type() == QEvent::Drop &&
         K3ColorDrag::canDecode( static_cast<QDropEvent *>(ev) ) )
    {
        dropEvent( static_cast<QDropEvent *>(ev) );
        return true;
    }

    if ( o == m_label )
    {
        QMouseEvent *e = (QMouseEvent *)ev;

        if ( ev->type() == QEvent::MouseButtonDblClick )
            slotRename();

        if ( ev->type() == QEvent::MouseButtonPress &&
             (e->button() == Qt::LeftButton || e->button() == Qt::MidButton))
        {
            e->button() == Qt::LeftButton ? KWin::raiseWindow( winId() )
                                      : KWin::lowerWindow( winId() );

            XUngrabPointer( QX11Info::display(), QX11Info::appTime() );
            NETRootInfo wm_root( QX11Info::display(), NET::WMMoveResize );
            wm_root.moveResizeRequest( winId(), e->globalX(), e->globalY(), NET::Move );
            return true;
        }

        if ( ev->type() == QEvent::MouseButtonRelease )
        {
            NETRootInfo wm_root( QX11Info::display(), NET::WMMoveResize );
            wm_root.moveResizeRequest( winId(), e->globalX(), e->globalY(), NET::MoveResizeCancel );
            return false;
        }

        return false;
    }

    if ( o == m_editor )
    {
        if ( ev->type() == QEvent::FocusOut )
        {
            QFocusEvent *fe = static_cast<QFocusEvent *>(ev);
            if ( fe->reason() != Qt::PopupFocusReason &&
                 fe->reason() != Qt::MouseFocusReason )
            {
                updateFocus();
                if ( isModified() )
                    saveData();
            }
        }
        else if ( ev->type() == QEvent::FocusIn )
            updateFocus();

        return false;
    }

    return false;
}


#include "knote.moc"
#include "knotebutton.moc"
