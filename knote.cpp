/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2007, The KNotes Developers

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

#include "knote.h"
#include "knotealarmdlg.h"
#include "knotebutton.h"
#include "knoteconfig.h"
#include "knoteconfigdlg.h"
#include "knoteedit.h"
#include "knotehostdlg.h"
#include "knoteprinter.h"
#include "knotesglobalconfig.h"
#include "knotesnetsend.h"
#include "kdepim-version.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kcal/journal.h>
#include <kcodecs.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kfind.h>
#include <kglobalsettings.h>
#include <kicon.h>
#include <kiconeffect.h>
#include <kiconloader.h>
#include <kinputdialog.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kselectaction.h>
#include <ksocketfactory.h>
#include <kstandardaction.h>
#include <kstandarddirs.h>
#include <ktoggleaction.h>
#include <ktoolbar.h>
#include <kwindowsystem.h>
#include <kxmlguibuilder.h>
#include <kxmlguifactory.h>
#include <netwm.h>

#include <QBoxLayout>
#include <QCheckBox>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QSize>
#include <QSizeGrip>
#include <QTextStream>
#include <QVBoxLayout>
#include <QDesktopWidget>

#ifdef Q_WS_X11
#include <fixx11h.h>
#include <QX11Info>
#endif

using namespace KCal;


KNote::KNote( const QDomDocument& buildDoc, Journal *j, QWidget *parent )
  : QFrame( parent, Qt::FramelessWindowHint ), m_label( 0 ), m_grip( 0 ),
    m_button( 0 ), m_tool( 0 ), m_editor( 0 ), m_config( 0 ), m_journal( j ),
    m_find( 0 ), m_kwinConf( KSharedConfig::openConfig( QLatin1String("kwinrc") ) ), m_blockEmitDataChanged( false ),mBlockWriteConfigDuringCommitData( false )
{
  setAcceptDrops( true );
  setAttribute( Qt::WA_DeleteOnClose );
  setDOMDocument( buildDoc );
  setObjectName( m_journal->uid() );
  setXMLFile( componentData().componentName() + QLatin1String("ui.rc"), false, false );

  // create the main layout
  m_noteLayout = new QVBoxLayout( this );
  m_noteLayout->setMargin( 0 );

  // if there is no title yet, use the start date if valid
  // (KOrganizer's journals don't have titles but a valid start date)
  if ( m_journal->summary().isNull() && m_journal->dtStart().isValid() ) {
    const QString s = KGlobal::locale()->formatDateTime( m_journal->dtStart() );
    m_journal->setSummary( s );
  }

  createActions();

  buildGui();

  prepare();
}

KNote::~KNote()
{
  delete m_config;
}

void KNote::changeJournal(KCal::Journal *journal)
{
   m_journal = journal;
   m_editor->setText( m_journal->description() );
   m_editor->document()->setModified( false );
   m_label->setText( m_journal->summary() );
   updateLabelAlignment();

}

// -------------------- public slots -------------------- //

void KNote::slotKill( bool force )
{
  m_blockEmitDataChanged = true;
  if ( !force &&
       ( KMessageBox::warningContinueCancel( this,
         i18n( "<qt>Do you really want to delete note <b>%1</b>?</qt>",
               m_label->text() ),
         i18n( "Confirm Delete" ),
         KGuiItem( i18n( "&Delete" ), QLatin1String("edit-delete") ),
         KStandardGuiItem::cancel(),
         QLatin1String("ConfirmDeleteNote") ) != KMessageBox::Continue ) ) {
     m_blockEmitDataChanged = false;
     return;
  }
  // delete the configuration first, then the corresponding file
  delete m_config;
  m_config = 0;
  QString configFile = KGlobal::dirs()->saveLocation( "appdata", QLatin1String("notes/") );
  configFile += m_journal->uid();
  if ( !KIO::NetAccess::del( KUrl( configFile ), this ) ) {
    kError( 5500 ) <<"Can't remove the note config:" << configFile;
  }

  emit sigKillNote( m_journal );
}


// -------------------- public member functions -------------------- //

void KNote::saveData(bool update )
{
  m_journal->setSummary( m_label->text() );
  m_journal->setDescription( m_editor->text() );
  m_journal->setCustomProperty( "KNotes", "FgColor",
                                m_config->fgColor().name() );
  m_journal->setCustomProperty( "KNotes", "BgColor",
                                m_config->bgColor().name() );
  m_journal->setCustomProperty( "KNotes", "RichText",
                                m_config->richText() ? QLatin1String("true") : QLatin1String("false") );

  if(update)
  {
     emit sigDataChanged(noteId());
     m_editor->document()->setModified( false );
  }
}

void KNote::saveConfig() const
{
  m_config->setWidth( width() );
  m_config->setHeight( height() );
  m_config->setPosition( pos() );

#ifdef Q_WS_X11
  NETWinInfo wm_client( QX11Info::display(), winId(),
                        QX11Info::appRootWindow(), NET::WMDesktop );
  if ( ( wm_client.desktop() == NETWinInfo::OnAllDesktops ) ||
     ( wm_client.desktop() > 0 ) ) {
    m_config->setDesktop( wm_client.desktop() );
  }
#endif

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

  if ( m_editor ) {    // not called from CTOR?
    saveData();
  }
#ifdef Q_WS_X11
  // set the window's name for the taskbar entry to be more helpful (#58338)
  NETWinInfo note_win( QX11Info::display(), winId(), QX11Info::appRootWindow(),
                       NET::WMDesktop );
  note_win.setName( name.toUtf8() );
#endif

  emit sigNameChanged(name);
}

void KNote::setText( const QString& text )
{
  m_editor->setText( text );

  saveData();
}

void KNote::find( KFind* kfind )
{
  m_find = kfind;
  disconnect( m_find );
  connect( m_find, SIGNAL(highlight(QString,int,int)),
           this, SLOT(slotHighlight(QString,int,int)) );
  connect( m_find, SIGNAL(findNext()), this, SLOT(slotFindNext()) );

  m_find->setData( m_editor->toPlainText() );
  slotFindNext();
}

bool KNote::isDesktopAssigned() const
{
  return m_config->rememberDesktop();
}

void KNote::slotFindNext()
{
  // TODO: honor FindBackwards

  // Let KFind inspect the text fragment, and display a dialog if a match is
  // found
  KFind::Result res = m_find->find();

  if ( res == KFind::NoMatch ) { // i.e. at end-pos

    QTextCursor c = m_editor->textCursor(); //doesn't return by reference, so we use setTextCursor
    c.clearSelection();
    m_editor->setTextCursor( c );

    disconnect( m_find, 0, this, 0 );
    emit sigFindFinished();
  } else {
      if (isHidden()) {
          show();
      } else {
          raise();
      }
#ifdef Q_WS_X11
    KWindowSystem::setCurrentDesktop( KWindowSystem::windowInfo( winId(),
                                      NET::WMDesktop ).desktop() );
#endif
  }
}

void KNote::slotHighlight( const QString& /*str*/, int idx, int len )
{
  m_editor->textCursor().clearSelection();
  m_editor->highlightWord( len, idx );

  // TODO: modify the selection color, use a different QTextCursor?
}

bool KNote::isModified() const
{
  return m_editor->document()->isModified();
}

// ------------------ private slots (menu actions) ------------------ //

void KNote::slotRename()
{
    m_blockEmitDataChanged = true;
  // pop up dialog to get the new name
  bool ok;
  const QString oldName = m_label->text();
  const QString newName = KInputDialog::getText( QString::null, //krazy:exclude=nullstrassign for old broken gcc
    i18n( "Please enter the new name:" ), m_label->text(), &ok, this );
  m_blockEmitDataChanged = false;
  if ( !ok || (oldName == newName) ) { // handle cancel
    return;
  }

  setName( newName );
}

void KNote::slotUpdateReadOnly()
{
  const bool readOnly = m_readOnly->isChecked();

  m_editor->setReadOnly( readOnly );
  m_config->setReadOnly( readOnly );

  // enable/disable actions accordingly
  actionCollection()->action( QLatin1String("configure_note") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("insert_date") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("delete_note") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_bold") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_italic") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_underline") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_strikeout") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_alignleft") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_aligncenter") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_alignright") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_alignblock" ))->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_list") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_super") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_sub") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_increaseindent" ))->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_decreaseindent") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("text_background_color") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_size") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("format_color") )->setEnabled( !readOnly );
  actionCollection()->action( QLatin1String("rename_note") )->setEnabled( !readOnly);

  updateFocus();
}


void KNote::commitData()
{
  mBlockWriteConfigDuringCommitData = true;
}

void KNote::slotClose()
{
#ifdef Q_WS_X11
  NETWinInfo wm_client( QX11Info::display(), winId(),
                        QX11Info::appRootWindow(), NET::WMDesktop );
  if ( ( wm_client.desktop() == NETWinInfo::OnAllDesktops ) ||
       ( wm_client.desktop() > 0 ) ) {
    m_config->setDesktop( wm_client.desktop() );
  }
#endif

  m_editor->clearFocus();
  if( !mBlockWriteConfigDuringCommitData )
  {
    m_config->setHideNote( true );
    m_config->setPosition( pos() );
    m_config->writeConfig();
  }
  // just hide the note so it's still available from the dock window
  hide();
}

void KNote::slotInsDate()
{
  m_editor->insertPlainText(
    KGlobal::locale()->formatDateTime( QDateTime::currentDateTime() ) );
}

void KNote::slotSetAlarm()
{
    m_blockEmitDataChanged = true;
  KNoteAlarmDlg dlg( name(), this );
  dlg.setIncidence( m_journal );

  if ( dlg.exec() == QDialog::Accepted ) {
    emit sigDataChanged(noteId());
  }
  m_blockEmitDataChanged = false;
}

void KNote::slotPreferences()
{
    m_blockEmitDataChanged = true;

  // create a new preferences dialog...
  QPointer<KNoteSimpleConfigDlg> dialog = new KNoteSimpleConfigDlg( m_config, name(), this, noteId() );
  connect( dialog, SIGNAL(settingsChanged(QString)) , this,
           SLOT(slotApplyConfig()) );
  connect( this, SIGNAL(sigNameChanged(QString)), dialog,
           SLOT(slotUpdateCaption(QString)) );
  dialog->exec();
  delete dialog;
  m_blockEmitDataChanged = false;
  saveData();
}

void KNote::slotSend()
{
  // pop up dialog to get the IP
  KNoteHostDlg hostDlg( i18n( "Send \"%1\"", name() ), this );
  const bool ok = ( hostDlg.exec() == QDialog::Accepted );

  if ( !ok ) { // handle cancel
    return;
  }
  const QString host = hostDlg.host();
  quint16 port = hostDlg.port();

  if ( !port ) { // not specified, use default
    port = KNotesGlobalConfig::port();
  }

  if ( host.isEmpty() ) {
    KMessageBox::sorry( this, i18n( "The host cannot be empty." ) );
    return;
  }

  // Send the note

  KNotesNetworkSender *sender = new KNotesNetworkSender(
    KSocketFactory::connectToHost( QLatin1String("knotes"), host, port ) );
  sender->setSenderId( KNotesGlobalConfig::senderID() );
  sender->setNote( name(), text() ); // FIXME: plainText ??
}

void KNote::slotMail()
{
  // get the mail action command
  const QStringList cmd_list = KNotesGlobalConfig::mailAction().split( QLatin1Char(' '),
      QString::SkipEmptyParts );

  KProcess mail;
  foreach ( const QString &cmd, cmd_list ) {
    if ( cmd == QLatin1String("%f") ) {
      mail << m_editor->toPlainText();
    } else if ( cmd == QLatin1String("%t") ) {
      mail << m_label->text();
    } else {
      mail << cmd;
    }
  }

  if ( !mail.startDetached() ) {
    KMessageBox::sorry( this, i18n( "Unable to start the mail process." ) );
  }
}

void KNote::slotPrint()
{
  QString content;
  if ( !Qt::mightBeRichText( m_editor->text() ) ) {
    content = Qt::convertFromPlainText( m_editor->text() );
  } else {
    content = m_editor->text();
  }
  KNotePrinter printer;
  printer.setDefaultFont( m_config->font() );
  printer.printNote( name(), content );
}

void KNote::slotSaveAs()
{
  // TODO: where to put pdf file support? In the printer??!??!
  m_blockEmitDataChanged = true;
  QCheckBox *convert = 0;

  if ( m_editor->acceptRichText() ) {
    convert = new QCheckBox( 0 );
    convert->setText( i18n( "Save note as plain text" ) );
  }
  m_blockEmitDataChanged = true;
  KUrl url;
  KFileDialog dlg( url, QString(), this, convert );
  dlg.setOperationMode( KFileDialog::Saving );
  dlg.setCaption( i18n( "Save As" ) );
  dlg.exec();

  QString fileName = dlg.selectedFile();
  if ( fileName.isEmpty() ) {
    m_blockEmitDataChanged = false;
    return;
  }

  QFile file( fileName );

  if ( file.exists() &&
       KMessageBox::warningContinueCancel( this,
          i18n( "<qt>A file named <b>%1</b> already exists.<br />"
                "Are you sure you want to overwrite it?</qt>",
                QFileInfo( file ).fileName() ) ) != KMessageBox::Continue ) {
    m_blockEmitDataChanged = false;
    return;
  }

  if ( file.open( QIODevice::WriteOnly ) ) {
    QTextStream stream( &file );
    if ( convert && !convert->isChecked() ) {
      stream << m_editor->toHtml();
    } else {
      stream << m_editor->toPlainText();
    }
  }
  m_blockEmitDataChanged = false;
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

  setColor( m_config->fgColor(), m_config->bgColor() );

  updateLayout();
  slotUpdateShowInTaskbar();
}

void KNote::slotKeepAbove()
{
    if ( m_keepBelow->isChecked() )
    {
        m_keepBelow->setChecked( false );
    }
    slotUpdateKeepAboveBelow();
}

void KNote::slotKeepBelow()
{
    if ( m_keepAbove->isChecked() )
    {
        m_keepAbove->setChecked( false );
    }
    slotUpdateKeepAboveBelow();
}

void KNote::slotUpdateKeepAboveBelow()
{
#ifdef Q_WS_X11
  unsigned long state = KWindowInfo( KWindowSystem::windowInfo( winId(), NET::WMState ) ).state();
#else
  unsigned long state = 0; // neutral state, TODO
#endif
  if ( m_keepAbove->isChecked() ) {
    m_config->setKeepAbove( true );
    m_config->setKeepBelow( false );
    KWindowSystem::setState( winId(), state | NET::KeepAbove );
  } else if ( m_keepBelow->isChecked() ) {
    m_config->setKeepAbove( false );
    m_config->setKeepBelow( true );
    KWindowSystem::setState( winId(), state | NET::KeepBelow );
  } else {
    m_config->setKeepAbove( false );
    KWindowSystem::clearState( winId(), NET::KeepAbove );
    m_config->setKeepBelow( false );
    KWindowSystem::clearState( winId(), NET::KeepBelow );
  }
}

void KNote::slotUpdateShowInTaskbar()
{
#ifdef Q_WS_X11
  if ( !m_config->showInTaskbar() ) {
    KWindowSystem::setState( winId(), KWindowSystem::windowInfo( winId(),
                             NET::WMState ).state() | NET::SkipTaskbar );
  } else {
    KWindowSystem::clearState( winId(), NET::SkipTaskbar );
  }
#endif
}

void KNote::slotUpdateDesktopActions()
{
#ifdef Q_WS_X11
  NETRootInfo wm_root( QX11Info::display(), NET::NumberOfDesktops |
                       NET::DesktopNames );
  NETWinInfo wm_client( QX11Info::display(), winId(),
                        QX11Info::appRootWindow(), NET::WMDesktop );

  QStringList desktops;
  desktops.append( i18n( "&All Desktops" ) );
  desktops.append( QString::null );           // Separator
                                              // krazy:exclude=nullstrassign
                                              // for old broken gcc

  const int count = wm_root.numberOfDesktops();
  for ( int n = 1; n <= count; ++n ) {
    desktops.append( QString::fromLatin1( "&%1 %2" ).arg( n ).arg(
      QString::fromUtf8( wm_root.desktopName( n ) ) ) );
  }
  m_toDesktop->setItems( desktops );

  if ( wm_client.desktop() == NETWinInfo::OnAllDesktops ) {
    m_toDesktop->setCurrentItem( 0 );
  } else {
    m_toDesktop->setCurrentItem( wm_client.desktop() + 1 ); // compensate for
                                                            // separator (+1)
  }
#endif
}


// -------------------- private methods -------------------- //

void KNote::buildGui()
{
  createNoteHeader();
  createNoteEditor();

  KXMLGUIBuilder builder( this );
  KXMLGUIFactory factory( &builder, this );
  factory.addClient( this );

  m_menu = dynamic_cast<KMenu*>( factory.container( QLatin1String("note_context"), this ) );
  m_tool = dynamic_cast<KToolBar*>( factory.container( QLatin1String("note_tool"), this ) );

  createNoteFooter();
}

void KNote::createActions()
{
  // create the menu items for the note - not the editor...
  // rename, mail, print, save as, insert date, alarm, close, delete, new note
  KAction *action;

  action  = new KAction( KIcon( QLatin1String("document-new") ), i18n( "New" ),  this );
  actionCollection()->addAction( QLatin1String("new_note"), action );
  connect( action, SIGNAL(triggered(bool)), SLOT(slotRequestNewNote()) );

  action  = new KAction( KIcon( QLatin1String("edit-rename") ), i18n( "Rename..." ), this );
  actionCollection()->addAction( QLatin1String("rename_note"), action );
  connect( action, SIGNAL(triggered(bool)), SLOT(slotRename()) );

  m_readOnly  = new KToggleAction( KIcon( QLatin1String("object-locked") ),
                                   i18n( "Lock" ), this );
  actionCollection()->addAction( QLatin1String("lock_note"), m_readOnly );
  connect( m_readOnly, SIGNAL(triggered(bool)),
          SLOT(slotUpdateReadOnly()) );
  m_readOnly->setCheckedState( KGuiItem( i18n( "Unlock" ), QLatin1String("object-unlocked") ) );

  action  = new KAction( KIcon( QLatin1String("window-close") ), i18n( "Hide" ), this );
  actionCollection()->addAction( QLatin1String("hide_note"), action );
  connect( action, SIGNAL(triggered(bool)), SLOT(slotClose()) );
  action->setShortcut( QKeySequence( Qt::Key_Escape ) );

  action  = new KAction( KIcon( QLatin1String("edit-delete") ), i18n( "Delete" ), this );
  actionCollection()->addAction( QLatin1String("delete_note"), action );
  connect( action, SIGNAL(triggered(bool)), SLOT(slotKill()),Qt::QueuedConnection );

  action  = new KAction( KIcon( QLatin1String("knotes_date") ), i18n( "Insert Date" ), this );
  actionCollection()->addAction( QLatin1String("insert_date"), action );
  connect( action, SIGNAL(triggered(bool)), SLOT(slotInsDate()) );

  action  = new KAction( KIcon( QLatin1String("knotes_alarm") ), i18n( "Set Alarm..." ),
                         this );
  actionCollection()->addAction( QLatin1String("set_alarm"), action );
  connect( action, SIGNAL(triggered(bool)), SLOT(slotSetAlarm()) );

  action  = new KAction( KIcon( QLatin1String("network-wired") ), i18n( "Send..." ), this );
  actionCollection()->addAction( QLatin1String("send_note"), action );
  connect( action, SIGNAL(triggered(bool)), SLOT(slotSend()) );

  action  = new KAction( KIcon( QLatin1String("mail-send") ), i18n( "Mail..." ), this );
  actionCollection()->addAction( QLatin1String("mail_note"), action );
  connect( action, SIGNAL(triggered(bool)), SLOT(slotMail()) );

  action  = new KAction( KIcon( QLatin1String("document-save-as") ), i18n( "Save As..." ),
                                this );
  actionCollection()->addAction( QLatin1String("save_note"), action );
  connect( action, SIGNAL(triggered(bool)), SLOT(slotSaveAs()) );
  actionCollection()->addAction( KStandardAction::Print,  QLatin1String("print_note"), this,
                                 SLOT(slotPrint()) );

  action  = new KAction( KIcon( QLatin1String("configure") ), i18n( "Preferences..." ), this );
  actionCollection()->addAction( QLatin1String("configure_note"), action );
  connect( action, SIGNAL(triggered(bool)), SLOT(slotPreferences()) );


  m_keepAbove  = new KToggleAction( KIcon( QLatin1String("go-up") ),
                                    i18n( "Keep Above Others" ), this );
  actionCollection()->addAction( QLatin1String("keep_above"), m_keepAbove );
  connect( m_keepAbove, SIGNAL(triggered(bool)),
           SLOT(slotKeepAbove()) );

  m_keepBelow  = new KToggleAction( KIcon( QLatin1String("go-down") ),
                                    i18n( "Keep Below Others" ), this );
  actionCollection()->addAction( QLatin1String("keep_below"), m_keepBelow );
  connect( m_keepBelow, SIGNAL(triggered(bool)),
           SLOT(slotKeepBelow()) );

#ifdef Q_WS_X11
  m_toDesktop  = new KSelectAction( i18n( "To Desktop" ), this );
  actionCollection()->addAction( QLatin1String("to_desktop"), m_toDesktop );
  connect( m_toDesktop, SIGNAL(triggered(int)),
           SLOT(slotPopupActionToDesktop(int)) );
  connect( m_toDesktop->menu(), SIGNAL(aboutToShow()),
           SLOT(slotUpdateDesktopActions()) );
  // initially populate it, otherwise stays disabled
  slotUpdateDesktopActions();
#endif

  // invisible action to walk through the notes to make this configurable
  action  = new KAction( i18n( "Walk Through Notes" ), this );
  actionCollection()->addAction( QLatin1String("walk_notes"), action );
  connect( action, SIGNAL(triggered(bool)), SIGNAL(sigShowNextNote()) );
  action->setShortcut( QKeySequence( Qt::SHIFT + Qt::Key_Backtab ) );

  actionCollection()->addAssociatedWidget( this );
  foreach (QAction* action, actionCollection()->actions())
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
}

void KNote::createNoteHeader()
{
  // load style configuration
  KConfigGroup styleGroup( m_kwinConf, "Style" );

  QBoxLayout::Direction headerLayoutDirection = QBoxLayout::LeftToRight;

  if ( styleGroup.readEntry( "CustomButtonPositions", false ) ) {
    if ( styleGroup.readEntry( "ButtonsOnLeft" ).contains( QLatin1Char('X') ) ) {
      headerLayoutDirection = QBoxLayout::RightToLeft;
    }
  }

  QBoxLayout *headerLayout = new QBoxLayout( headerLayoutDirection);


  // create header label
  m_label = new QLabel( this );
  headerLayout->addWidget( m_label );
  m_label->setFrameStyle( NoFrame );
  m_label->setBackgroundRole( QPalette::Base );
  m_label->setLineWidth( 0 );
  m_label->setAutoFillBackground( true );
  m_label->installEventFilter( this );  // receive events ( for dragging &
                                        // action menu )
  setName( m_journal->summary() );      // don't worry, no signals are
                                        // connected at this stage yet
  m_button = new KNoteButton( QLatin1String("knotes_close"), this );
  headerLayout->addWidget( m_button );

  connect( m_button, SIGNAL(clicked()), this, SLOT(slotClose()) );

  m_noteLayout->addItem( headerLayout );
}

void KNote::createNoteEditor()
{
  m_editor = new KNoteEdit( actionCollection(), this );
  m_noteLayout->addWidget( m_editor );
  m_editor->setNote( this );
  m_editor->installEventFilter( this ); // receive focus events for modified
  setFocusProxy( m_editor );
}

void KNote::slotRequestNewNote()
{
    //Be sure to save before to request a new note
    saveConfig();
    saveData();
    emit sigRequestNewNote();
}

void KNote::createNoteFooter()
{
  if ( m_tool ) {
    m_tool->setIconSize( QSize( 10, 10 ) );
    m_tool->setFixedHeight( 24 );
    m_tool->setToolButtonStyle( Qt::ToolButtonIconOnly );
  }

  // create size grip
  QHBoxLayout *gripLayout = new QHBoxLayout;
  m_grip = new QSizeGrip( this );
  m_grip->setFixedSize( m_grip->sizeHint() );

  if ( m_tool ) {
    gripLayout->addWidget( m_tool );
    gripLayout->setAlignment( m_tool, Qt::AlignBottom | Qt::AlignLeft );
    m_tool->hide();
  }

  gripLayout->addWidget( m_grip );
  gripLayout->setAlignment( m_grip, Qt::AlignBottom | Qt::AlignRight );
  m_noteLayout->addItem( gripLayout );

  // if there was just a way of making KComboBox adhere the toolbar height...
  if ( m_tool ) {
    foreach ( KComboBox *combo, m_tool->findChildren<KComboBox *>() ) {
      QFont font = combo->font();
      font.setPointSize( 7 );
      combo->setFont( font );
      combo->setFixedHeight( 14 );
    }
  }
}

void KNote::prepare()
{
  // the config file location
  const QString configFile = KGlobal::dirs()->saveLocation( "appdata", QLatin1String("notes/") ) + m_journal->uid();

  // no config file yet? -> use the default display config if available
  // we want to write to configFile, so use "false"
  const bool newNote = !KIO::NetAccess::exists( KUrl( configFile ),
                                          KIO::NetAccess::DestinationSide, 0 );

  m_config = new KNoteConfig( KSharedConfig::openConfig( configFile,
                                                         KConfig::NoGlobals ) );
  m_config->readConfig();
  m_config->setVersion( QLatin1String(KDEPIM_VERSION) );

  if ( newNote ) {
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
    m_config->setRememberDesktop( globalConfig->rememberDesktop() );
    m_config->setKeepAbove( globalConfig->keepAbove() );
    m_config->setKeepBelow( globalConfig->keepBelow() );

    m_config->writeConfig();
  }

  // load the display configuration of the note
  uint width = m_config->width();
  uint height = m_config->height();
  resize( width, height );

  // let KWin do the placement if the position is illegal--at least 10 pixels
  // of a note need to be visible
  const QPoint& position = m_config->position();
  QRect desk = kapp->desktop()->rect();
  desk.adjust( 10, 10, -10, -10 );
  if ( desk.intersects( QRect( position, QSize( width, height ) ) ) ) {
    move( position );           // do before calling show() to avoid flicker
  }

  // config items in the journal have priority
  QString property = m_journal->customProperty( "KNotes", "FgColor" );
  if ( !property.isNull() ) {
    m_config->setFgColor( QColor( property ) );
  } else {
    m_journal->setCustomProperty( "KNotes", "FgColor",
                                  m_config->fgColor().name() );
  }

  property = m_journal->customProperty( "KNotes", "BgColor" );
  if ( !property.isNull() ) {
    m_config->setBgColor( QColor( property ) );
  } else {
    m_journal->setCustomProperty( "KNotes", "BgColor",
                                  m_config->bgColor().name() );
  }
  property = m_journal->customProperty( "KNotes", "RichText" );
  if ( !property.isNull() ) {
    m_config->setRichText( property == QLatin1String("true") ? true : false );
  } else {
    m_journal->setCustomProperty( "KNotes", "RichText",
                                  m_config->richText() ? QLatin1String("true") : QLatin1String("false") );
  }

  // read configuration settings...
  slotApplyConfig();

  // if this is a new note put on current desktop - we can't use defaults
  // in KConfig XT since only _changes_ will be stored in the config file
  int desktop = m_config->desktop();

#ifdef Q_WS_X11
  if ( ( desktop < 0 && desktop != NETWinInfo::OnAllDesktops ) ||
       !m_config->rememberDesktop() )
    desktop = KWindowSystem::currentDesktop();
#endif

  // show the note if desired
  if ( desktop != 0 && !m_config->hideNote() ) {
    // to avoid flicker, call this before show()
    toDesktop( desktop );
    show();

    // because KWin forgets about that for hidden windows
#ifdef Q_WS_X11
    if ( desktop == NETWinInfo::OnAllDesktops ) {
      toDesktop( desktop );
    }
#endif
  }

  m_readOnly->setChecked( m_config->readOnly() );
  slotUpdateReadOnly();

  if ( m_config->keepAbove() ) {
    m_keepAbove->setChecked( true );
  } else if ( m_config->keepBelow() ) {
    m_keepBelow->setChecked( true );
  } else {
    m_keepAbove->setChecked( false );
    m_keepBelow->setChecked( false );
  }
  slotUpdateKeepAboveBelow();

  // HACK: update the icon color - again after showing the note, to make kicker
  // aware of the new colors
  KIconEffect effect;
  QPixmap icon = effect.apply( qApp->windowIcon().pixmap(
                                 IconSize( KIconLoader::Desktop ),
                                 IconSize( KIconLoader::Desktop ) ),
                               KIconEffect::Colorize,
                               1, m_config->bgColor(), false );
  QPixmap miniIcon = effect.apply( qApp->windowIcon().pixmap(
                                     IconSize( KIconLoader::Small ),
                                     IconSize( KIconLoader::Small ) ),
                                   KIconEffect::Colorize,
                                   1, m_config->bgColor(), false );
#ifdef Q_WS_X11
  KWindowSystem::setIcons( winId(), icon, miniIcon );
#endif

  // set up the look&feel of the note
  setFrameStyle( Panel | Raised );
  setMinimumSize( 20, 20 );
  setBackgroundRole( QPalette::Base );

  m_editor->setContentsMargins( 0, 0, 0, 0 );
  m_editor->setBackgroundRole( QPalette::Base );
  m_editor->setFrameStyle( NoFrame );
  m_editor->setText( m_journal->description() );

  m_editor->document()->setModified( false );
}

void KNote::toDesktop( int desktop )
{
  if ( desktop == 0 ) {
    return;
  }

#ifdef Q_WS_X11
  if ( desktop == NETWinInfo::OnAllDesktops ) {
    KWindowSystem::setOnAllDesktops( winId(), true );
  } else {
    KWindowSystem::setOnDesktop( winId(), desktop );
  }
#endif
}

void KNote::setColor( const QColor &fg, const QColor &bg )
{
  QPalette p = palette();

  // better: from light(150) to light(100) to light(75)
  // QLinearGradient g( width()/2, 0, width()/2, height() );
  // g.setColorAt( 0, bg );
  // g.setColorAt( 1, bg.dark(150) );

  p.setColor( QPalette::Window,     bg );
  // p.setBrush( QPalette::Window,     g );
  p.setColor( QPalette::Base,       bg );
  // p.setBrush( QPalette::Base,       g );

  p.setColor( QPalette::WindowText, fg );
  p.setColor( QPalette::Text,       fg );

  p.setColor( QPalette::Button,     bg.dark( 116 ) );
  p.setColor( QPalette::ButtonText, fg );

  //p.setColor( QPalette::Highlight,  bg );
  //p.setColor( QPalette::HighlightedText, fg );

  // order: Light, Midlight, Button, Mid, Dark, Shadow

  // the shadow
  p.setColor( QPalette::Light, bg.light( 180 ) );
  p.setColor( QPalette::Midlight, bg.light( 150 ) );
  p.setColor( QPalette::Mid, bg.light( 150 ) );
  p.setColor( QPalette::Dark, bg.dark( 108 ) );
  p.setColor( QPalette::Shadow, bg.dark( 116 ) );

  setPalette( p );

  // darker values for the active label
  p.setColor( QPalette::Active, QPalette::Base, bg.dark( 116 ) );

  m_label->setPalette( p );

  // set the text color
  m_editor->setTextColor( fg );

  // update the icon color
  KIconEffect effect;
  QPixmap icon = effect.apply( qApp->windowIcon().pixmap(
                                 IconSize( KIconLoader::Desktop ),
                                 IconSize( KIconLoader::Desktop ) ),
                               KIconEffect::Colorize, 1, bg, false );
  QPixmap miniIcon = effect.apply( qApp->windowIcon().pixmap(
                                     IconSize( KIconLoader::Small ),
                                     IconSize( KIconLoader::Small ) ),
                                   KIconEffect::Colorize, 1, bg, false );
#ifdef Q_WS_X11
  KWindowSystem::setIcons( winId(), icon, miniIcon );
#endif
  // update the color of the title
  updateFocus();
  emit sigColorChanged();
}

void KNote::updateLabelAlignment()
{
  // if the name is too long to fit, left-align it, otherwise center it (#59028)
  const QString labelText = m_label->text();
  if ( m_label->fontMetrics().boundingRect( labelText ).width() >
       m_label->width() ) {
    m_label->setAlignment( Qt::AlignLeft );
  } else {
    m_label->setAlignment( Qt::AlignHCenter );
  }
}

void KNote::updateFocus()
{
  if ( hasFocus() )
  {

    if ( !m_editor->isReadOnly() )
    {
        if ( m_tool && m_tool->isHidden() && m_editor->acceptRichText() )
        {
            m_tool->show();
            updateLayout();
        }
        m_grip->show();
    }
    else
    {
        if ( m_tool && !m_tool->isHidden() ) {
            m_tool->hide();
            updateLayout();     // to update the minimum height
        }
        m_grip->hide();
    }
  }
  else
  {
    m_grip->hide();

    if ( m_tool && !m_tool->isHidden() )
    {
      m_tool->hide();
      updateLayout();     // to update the minimum height
    }
  }
}

void KNote::updateLayout()
{
  // TODO: remove later if no longer needed.
  updateLabelAlignment();
}

// -------------------- protected methods -------------------- //

void KNote::contextMenuEvent( QContextMenuEvent *e )
{
  if ( m_menu ) {
    m_menu->popup( e->globalPos() );
  }
}

void KNote::showEvent( QShowEvent * )
{
  if ( m_config->hideNote() ) {
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

void KNote::closeEvent( QCloseEvent * event )
{
  if ( kapp->sessionSaving() ) {
    return;
  }
  event->ignore(); //We don't want to close (and delete the widget). Just hide it
  slotClose();
}

void KNote::dragEnterEvent( QDragEnterEvent *e )
{
  if ( !m_config->readOnly() ) {
    e->setAccepted( e->mimeData()->hasColor() );
  }
}

void KNote::dropEvent( QDropEvent *e )
{
  if ( m_config->readOnly() ) {
    return;
  }

  const QMimeData *md = e->mimeData();
  if ( md->hasColor() ) {
       QColor bg =  qvariant_cast<QColor>( md->colorData() );
       setColor( palette().color( foregroundRole() ), bg );
       m_journal->setCustomProperty( "KNotes", "BgColor", bg.name() );
       m_config->setBgColor( bg );
  }
}

bool KNote::event( QEvent *ev )
{
  if ( ev->type() == QEvent::LayoutRequest ) {
    updateLayout();
    return true;
  } else {
    return QFrame::event( ev );
  }
}

bool KNote::eventFilter( QObject *o, QEvent *ev )
{
  if ( ev->type() == QEvent::DragEnter &&
    static_cast<QDragEnterEvent*>( ev )->mimeData()->hasColor() ) {
    dragEnterEvent( static_cast<QDragEnterEvent *>( ev ) );
    return true;
  }

  if ( ev->type() == QEvent::Drop &&
       static_cast<QDropEvent *>( ev )->mimeData()->hasColor() ) {
    dropEvent( static_cast<QDropEvent *>( ev ) );
    return true;
  }

  if ( o == m_label ) {
    QMouseEvent *e = ( QMouseEvent * )ev;

    if ( ev->type() == QEvent::MouseButtonDblClick ) {
       if(!m_editor->isReadOnly())
          slotRename();
    }

    if ( ev->type() == QEvent::MouseButtonPress &&
        ( e->button() == Qt::LeftButton || e->button() == Qt::MidButton ) ) {
#ifdef Q_WS_X11
      e->button() == Qt::LeftButton ? KWindowSystem::raiseWindow( winId() )
                                    : KWindowSystem::lowerWindow( winId() );

      XUngrabPointer( QX11Info::display(), QX11Info::appTime() );
      NETRootInfo wm_root( QX11Info::display(), NET::WMMoveResize );
      wm_root.moveResizeRequest( winId(), e->globalX(), e->globalY(),
                                 NET::Move );
#endif
      return true;
    }

    if ( ev->type() == QEvent::MouseButtonRelease ) {
#ifdef Q_WS_X11
        NETRootInfo wm_root( QX11Info::display(), NET::WMMoveResize );
        wm_root.moveResizeRequest( winId(), e->globalX(), e->globalY(),
                                   NET::MoveResizeCancel );
#endif
        return false;
    }

    return false;
  }

  if ( o == m_editor ) {
    if ( ev->type() == QEvent::FocusOut ) {
          QFocusEvent *fe = static_cast<QFocusEvent *>( ev );
          if ( fe->reason() != Qt::PopupFocusReason &&
               fe->reason() != Qt::MouseFocusReason ) {
            updateFocus();
            if ( isModified() ) {
              saveConfig();
              if ( !m_blockEmitDataChanged )
                  saveData();
            }
          }
    } else if ( ev->type() == QEvent::FocusIn ) {
      updateFocus();
    }

    return false;
  }

  return false;
}


#include "knote.moc"
