/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2013, The KNotes Developers

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
#include "config-kdepim.h"
#include "knote.h"
#include "noteshared/noteutils.h"
#include "alarms/notealarmdialog.h"
#include "configdialog/knotesimpleconfigdialog.h"
#include "print/knoteprintselectthemedialog.h"
#include "knotes/notes/knotebutton.h"
#include "utils/knoteutils.h"
#include "configdialog/knoteconfigdialog.h"
#include "knoteedit.h"
#include "print/knoteprinter.h"
#include "print/knoteprintobject.h"
#include "knotesglobalconfig.h"

#include "knotedisplaysettings.h"

#include "noteshared/attributes/notelockattribute.h"
#include "noteshared/attributes/notedisplayattribute.h"
#include "noteshared/attributes/notealarmattribute.h"


#include <KMime/KMimeMessage>

#include <kactioncollection.h>
#include <kapplication.h>
#include <kcombobox.h>
#include <kfiledialog.h>
#include <kiconeffect.h>
#include <kiconloader.h>
#include <qinputdialog.h>
#include <KLocalizedString>
#include <QMenu>
#include <kmessagebox.h>
#include <kselectaction.h>
#include <kstandardaction.h>

#include <ktoggleaction.h>
#include <ktoolbar.h>
#include <kwindowsystem.h>
#include <kxmlguibuilder.h>
#include <kxmlguifactory.h>
#include <netwm.h>
#include <KPrintPreview>

#include <AkonadiCore/ItemModifyJob>

#include <QBoxLayout>
#include <QCheckBox>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QSize>
#include <QSizeGrip>
#include <QTextStream>
#include <QVBoxLayout>
#include <QPointer>
#include <QFocusEvent>
#include <QMimeData>
#include <QDesktopWidget>

#if KDEPIM_HAVE_X11
#include <fixx11h.h>
#include <QX11Info>
#endif

//#define DEBUG_SAVE_NOTE 1

KNote::KNote(const QDomDocument& buildDoc, const Akonadi::Item &item, QWidget *parent )
    : QFrame( parent, Qt::FramelessWindowHint ),
      mItem(item),
      m_label( 0 ),
      m_grip( 0 ),
      m_button( 0 ),
      m_tool( 0 ),
      m_editor( 0 ),
      m_kwinConf( KSharedConfig::openConfig( QLatin1String("kwinrc") ) ),
      mDisplayAttribute(new KNoteDisplaySettings)
{
    if ( mItem.hasAttribute<NoteShared::NoteDisplayAttribute>()) {
        mDisplayAttribute->setDisplayAttribute(mItem.attribute<NoteShared::NoteDisplayAttribute>());
    } else {
        setDisplayDefaultValue();
        //save default display value
    }
    setAcceptDrops( true );
    setAttribute( Qt::WA_DeleteOnClose );
    setDOMDocument( buildDoc );
    //QT5 setXMLFile( componentData().componentName() + QLatin1String("ui.rc"), false, false );

    // create the main layout
    m_noteLayout = new QVBoxLayout( this );
    m_noteLayout->setMargin( 0 );
    createActions();

    buildGui();
    prepare();
}

KNote::~KNote()
{
    delete mDisplayAttribute;
}

void KNote::setDisplayDefaultValue()
{
    KNoteUtils::setDefaultValue(mItem);
    Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
    qDebug()<<"setDisplayDefaultValue slotNoteSaved(KJob*)";
#endif
    connect( job, SIGNAL(result(KJob*)), SLOT(slotNoteSaved(KJob*)) );
}

void KNote::setChangeItem(const Akonadi::Item &item, const QSet<QByteArray> &set)
{
    mItem = item;
    if ( item.hasAttribute<NoteShared::NoteDisplayAttribute>()) {
        mDisplayAttribute->setDisplayAttribute(item.attribute<NoteShared::NoteDisplayAttribute>());
    }
    if (set.contains("ATR:KJotsLockAttribute")) {
        m_editor->setReadOnly(item.hasAttribute<NoteShared::NoteLockAttribute>());
    }
    if (set.contains("PLD:RFC822")) {
        loadNoteContent(item);
    }
    if (set.contains("ATR:NoteDisplayAttribute")) {
        qDebug()<<" ATR:NoteDisplayAttribute";
        slotApplyConfig();
    }
    //TODO update display/content etc.
    updateLabelAlignment();
}

void KNote::slotKill( bool force )
{
    if ( !force &&
         ( KMessageBox::warningContinueCancel( this,
                                               i18n( "<qt>Do you really want to delete note <b>%1</b>?</qt>",
                                                     m_label->text() ),
                                               i18n( "Confirm Delete" ),
                                               KGuiItem( i18n( "&Delete" ), QLatin1String("edit-delete") ),
                                               KStandardGuiItem::cancel(),
                                               QLatin1String("ConfirmDeleteNote") ) != KMessageBox::Continue ) ) {
        return;
    }

    Q_EMIT sigKillNote( mItem.id() );
}


// -------------------- public member functions -------------------- //

void KNote::saveNote(bool force, bool sync)
{
    if (!force && !m_editor->document()->isModified())
        return;
    bool needToSave = false;
    NoteShared::NoteDisplayAttribute *attribute =  mItem.attribute<NoteShared::NoteDisplayAttribute>( Akonadi::Entity::AddIfMissing );
    const QPoint notePosition = pos();
    if (attribute->position() != notePosition) {
        needToSave = true;
        attribute->setPosition(notePosition);
    }
    const QSize currentSize(QSize(width(), height()));
    if (attribute->size() != currentSize) {
        needToSave = true;
        attribute->setSize(currentSize);
    }
#if KDEPIM_HAVE_X11
#if 0 //QT5
    NETWinInfo wm_client( QX11Info::display(), winId(),
                          QX11Info::appRootWindow(), NET::WMDesktop );
    if ( ( wm_client.desktop() == NETWinInfo::OnAllDesktops ) ||
         ( wm_client.desktop() > 0 ) ) {
        const int desktopNumber = wm_client.desktop();
        if (attribute->desktop() != desktopNumber) {
            needToSave = true;
            attribute->setDesktop( desktopNumber );
        }
    }
#endif
#endif
    if (m_editor->document()->isModified()) {
        needToSave = true;
        saveNoteContent();
    }
    if (needToSave) {
#ifdef DEBUG_SAVE_NOTE
        qDebug()<<"save Note slotClose() slotNoteSaved(KJob*) : sync"<<sync;
#endif
        Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(mItem);
        if (sync) {
            job->exec();
        } else {
#ifdef DEBUG_SAVE_NOTE
            qDebug()<<"save Note slotClose() slotNoteSaved(KJob*)";
#endif
            connect( job, SIGNAL(result(KJob*)), SLOT(slotNoteSaved(KJob*)) );
        }
    }
}

void KNote::slotNoteSaved(KJob *job)
{
    qDebug()<<" void KNote::slotNoteSaved(KJob *job)";
    if ( job->error() ) {
        qDebug()<<" problem during save note:"<<job->errorString();
    } else {
        m_editor->document()->setModified( false );
    }
}

Akonadi::Item::Id KNote::noteId() const
{
    return mItem.id();
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
        saveNote();
    }
#if 0 //QT5
#if KDEPIM_HAVE_X11
    // set the window's name for the taskbar entry to be more helpful (#58338)
    NETWinInfo note_win( QX11Info::display(), winId(), QX11Info::appRootWindow(),
                         NET::WMDesktop );
    note_win.setName( name.toUtf8() );
#endif
#endif

    emit sigNameChanged(name);
}

void KNote::setText( const QString& text )
{
    m_editor->setText( text );

    saveNote();
}

bool KNote::isDesktopAssigned() const
{
    return mDisplayAttribute->rememberDesktop();
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
    const QString oldName = m_label->text();
    const QString newName = QInputDialog::getText( this, QString::null, //krazy:exclude=nullstrassign for old broken gcc
                                                   i18n( "Please enter the new name:" ), QLineEdit::Normal, m_label->text(), &ok );
    if ( !ok || (oldName == newName) ) { // handle cancel
        return;
    }

    setName( newName );
}

void KNote::slotUpdateReadOnly()
{
    const bool readOnly = m_readOnly->isChecked();

    m_editor->setReadOnly( readOnly );

    if (mItem.hasAttribute<NoteShared::NoteLockAttribute>()) {
        if (!readOnly) {
            mItem.removeAttribute<NoteShared::NoteLockAttribute>();
        }
    } else {
        if (readOnly) {
            mItem.attribute<NoteShared::NoteLockAttribute>( Akonadi::Entity::AddIfMissing );
        }
    }
    if (!mBlockSave) {
        updateAllAttributes();
        Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
        qDebug()<<" void KNote::slotUpdateReadOnly() slotNoteSaved(KJob*)";
#endif
        connect( job, SIGNAL(result(KJob*)), SLOT(slotNoteSaved(KJob*)) );
    }


    // enable/disable actions accordingly
    actionCollection()->action( QLatin1String("configure_note") )->setEnabled( !readOnly );
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
    actionCollection()->action( QLatin1String("set_alarm") )->setEnabled( !readOnly);
    m_keepAbove->setEnabled( !readOnly);
    m_keepBelow->setEnabled( !readOnly);

#if KDEPIM_HAVE_X11
    m_toDesktop->setEnabled( !readOnly);
#endif

    updateFocus();
}

void KNote::updateAllAttributes()
{
    NoteShared::NoteDisplayAttribute *attribute =  mItem.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Entity::AddIfMissing);
#if 0 //QT5
#if KDEPIM_HAVE_X11
    NETWinInfo wm_client( QX11Info::display(), winId(),
                          QX11Info::appRootWindow(), NET::WMDesktop );
    if ( ( wm_client.desktop() == NETWinInfo::OnAllDesktops ) ||
         ( wm_client.desktop() > 0 ) ) {
        attribute->setDesktop(wm_client.desktop());
    }
#endif
#endif
    saveNoteContent();
    attribute->setIsHidden(true);
    attribute->setPosition(pos());
    const QSize currentSize(QSize(width(), height()));
    if (attribute->size() != currentSize) {
        attribute->setSize(currentSize);
    }
}

void KNote::slotClose()
{
    updateAllAttributes();
    m_editor->clearFocus();
    Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
    qDebug()<<"slotClose() slotNoteSaved(KJob*)";
#endif
    connect( job, SIGNAL(result(KJob*)), SLOT(slotNoteSaved(KJob*)) );
    hide();
}

void KNote::slotSetAlarm()
{
    QPointer<NoteShared::NoteAlarmDialog> dlg = new NoteShared::NoteAlarmDialog( name(), this );
    if (mItem.hasAttribute<NoteShared::NoteAlarmAttribute>()) {
        dlg->setAlarm(mItem.attribute<NoteShared::NoteAlarmAttribute>()->dateTime());
    }
    if ( dlg->exec() ) {
        bool needToModify = true;
        KDateTime dateTime = dlg->alarm();
        if (dateTime.isValid()) {
            NoteShared::NoteAlarmAttribute *attribute =  mItem.attribute<NoteShared::NoteAlarmAttribute>( Akonadi::Entity::AddIfMissing );
            attribute->setDateTime(dateTime);
        } else {
            if (mItem.hasAttribute<NoteShared::NoteAlarmAttribute>()) {
                mItem.removeAttribute<NoteShared::NoteAlarmAttribute>();
            } else {
                needToModify = false;
            }
        }
        if (needToModify) {
            //Verify it!
            saveNoteContent();
            Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
            qDebug()<<"setAlarm() slotNoteSaved(KJob*)";
#endif
            connect( job, SIGNAL(result(KJob*)), SLOT(slotNoteSaved(KJob*)) );
        }
    }
    delete dlg;
}

void KNote::saveNoteContent()
{    
    KMime::Message::Ptr message = mItem.payload<KMime::Message::Ptr>();
    const QByteArray encoding( "utf-8" );
    message->subject( true )->fromUnicodeString( name(), encoding );
    message->contentType( true )->setMimeType( m_editor->acceptRichText() ? "text/html" : "text/plain" );
    message->contentType()->setCharset(encoding);
    message->contentTransferEncoding(true)->setEncoding(KMime::Headers::CEquPr);
    message->date( true )->setDateTime( QDateTime::currentDateTime() );
    message->mainBodyPart()->fromUnicodeString( text().isEmpty() ? QString::fromLatin1( " " ) : text());

    KMime::Headers::Generic *header = new KMime::Headers::Generic( "X-Cursor-Position", message.get(), QString::number( m_editor->cursorPositionFromStart() ), "utf-8" );
    message->setHeader( header );

    message->assemble();

    mItem.setPayload( message );
}

void KNote::slotPreferences()
{
    // create a new preferences dialog...
    QPointer<KNoteSimpleConfigDialog> dialog = new KNoteSimpleConfigDialog( name(), this );
    NoteShared::NoteDisplayAttribute *attribute =  mItem.attribute<NoteShared::NoteDisplayAttribute>( Akonadi::Entity::AddIfMissing );
    attribute->setSize(QSize(width(), height()));

    dialog->load(mItem, m_editor->acceptRichText());
    connect( this, SIGNAL(sigNameChanged(QString)), dialog,
             SLOT(slotUpdateCaption(QString)) );
    if (dialog->exec() ) {
        bool isRichText;
        dialog->save(mItem, isRichText);
        m_editor->setAcceptRichText(isRichText);
        saveNoteContent();
        Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
        qDebug()<<"slotPreference slotNoteSaved(KJob*)";
#endif
        connect( job, SIGNAL(result(KJob*)), SLOT(slotNoteSaved(KJob*)) );
    }
    delete dialog;
}

void KNote::slotSend()
{
    NoteShared::NoteUtils::sendToNetwork(this, name(), text());
}

void KNote::slotMail()
{
    NoteShared::NoteUtils::sendToMail(this, m_label->text(), m_editor->toPlainText());
}

void KNote::slotPrint()
{
    print(false);
}

void KNote::slotPrintPreview()
{
    print(true);
}

void KNote::print(bool preview)
{
    QString content;
    if ( !Qt::mightBeRichText( m_editor->text() ) ) {
        content = Qt::convertFromPlainText( m_editor->text() );
    } else {
        content = m_editor->text();
    }
    if ( isModified() ) {
        saveNote();
    }
    KNotePrinter printer;
    QList<KNotePrintObject*> lst;
    lst.append(new KNotePrintObject(mItem));
    printer.setDefaultFont( mDisplayAttribute->font() );

    KNotesGlobalConfig *globalConfig = KNotesGlobalConfig::self();
    QString printingTheme = globalConfig->theme();
    if (printingTheme.isEmpty()) {
        QPointer<KNotePrintSelectThemeDialog> dlg = new KNotePrintSelectThemeDialog(this);
        if (dlg->exec()) {
            printingTheme = dlg->selectedTheme();
        }
        delete dlg;
    }
    if (!printingTheme.isEmpty()) {
        printer.printNotes( lst, printingTheme, preview );
    }
}

void KNote::slotSaveAs()
{
    // TODO: where to put pdf file support? In the printer??!??!
    QCheckBox *convert = 0;
    if ( m_editor->acceptRichText() ) {
        convert = new QCheckBox( 0 );
        convert->setText( i18n( "Save note as plain text" ) );
    }
    QUrl url;
    QPointer<KFileDialog> dlg = new KFileDialog( url, QString(), this, convert );
    dlg->setOperationMode( KFileDialog::Saving );
    dlg->setWindowTitle( i18n( "Save As" ) );
    if( !dlg->exec() ) {
        delete dlg;
        return;
    }

    const QString fileName = dlg->selectedFile();
    const bool htmlFormatAndSaveAsHtml = (convert && !convert->isChecked());
    delete dlg;
    if ( fileName.isEmpty() ) {
        return;
    }

    QFile file( fileName );

    if ( file.exists() &&
         KMessageBox::warningContinueCancel( this,
                                             i18n( "<qt>A file named <b>%1</b> already exists.<br />"
                                                   "Are you sure you want to overwrite it?</qt>",
                                                   QFileInfo( file ).fileName() ) ) != KMessageBox::Continue ) {
        return;
    }

    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &file );
        if ( htmlFormatAndSaveAsHtml ) {
            QString htmlStr = m_editor->toHtml();
            htmlStr.replace(QLatin1String("meta name=\"qrichtext\" content=\"1\""), QLatin1String("meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\""));
            stream <<  htmlStr;
        } else {
            stream << m_editor->toPlainText();
        }
    }
}

void KNote::slotPopupActionToDesktop( int id )
{
    toDesktop( id - 1 ); // compensate for the menu separator, -1 == all desktops
}


// ------------------ private slots (configuration) ------------------ //

void KNote::slotApplyConfig()
{
    m_label->setFont( mDisplayAttribute->titleFont() );
    m_editor->setTextFont( mDisplayAttribute->font() );
    m_editor->setTabStop( mDisplayAttribute->tabSize() );
    m_editor->setAutoIndentMode( mDisplayAttribute->autoIndent() );

    setColor( mDisplayAttribute->foregroundColor(), mDisplayAttribute->backgroundColor() );

    updateLayout();
    slotUpdateShowInTaskbar();
    resize(mDisplayAttribute->size());
}


void KNote::slotKeepAbove()
{
    if ( m_keepBelow->isChecked() ) {
        m_keepBelow->setChecked( false );
    }
    slotUpdateKeepAboveBelow();
}

void KNote::slotKeepBelow()
{
    if ( m_keepAbove->isChecked() ) {
        m_keepAbove->setChecked( false );
    }
    slotUpdateKeepAboveBelow();
}

void KNote::slotUpdateKeepAboveBelow(bool save)
{
#if KDEPIM_HAVE_X11
    NET::States state = KWindowInfo( KWindowSystem::windowInfo( winId(), NET::WMState ) ).state();
#else
    NET::States state = 0; // neutral state, TODO
#endif
    NoteShared::NoteDisplayAttribute *attribute =  mItem.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Entity::AddIfMissing);
    if ( m_keepAbove->isChecked() ) {
        attribute->setKeepAbove(true);
        attribute->setKeepBelow(false);
        KWindowSystem::setState( winId(), state | NET::KeepAbove );
    } else if ( m_keepBelow->isChecked() ) {
        attribute->setKeepAbove(false);
        attribute->setKeepBelow(true);
        //QT5 KWindowSystem::setState( winId(), state | NET::KeepBelow );
    } else {
        attribute->setKeepAbove(false);
        attribute->setKeepBelow(false);
        KWindowSystem::clearState( winId(), NET::KeepAbove );
        KWindowSystem::clearState( winId(), NET::KeepBelow );
    }
    if (!mBlockSave && save) {
        saveNoteContent();
        Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
        qDebug()<<"slotUpdateKeepAboveBelow slotNoteSaved(KJob*)";
#endif
        connect( job, SIGNAL(result(KJob*)), SLOT(slotNoteSaved(KJob*)) );
    }
}

void KNote::slotUpdateShowInTaskbar()
{
#if 0 //QT5
#if KDEPIM_HAVE_X11
    if ( !mDisplayAttribute->showInTaskbar() ) {
        KWindowSystem::setState( winId(), KWindowSystem::windowInfo( winId(),
                                                                     NET::WMState ).state() | NET::SkipTaskbar );
    } else {
        KWindowSystem::clearState( winId(), NET::SkipTaskbar );
    }
#endif
#endif
}

void KNote::slotUpdateDesktopActions()
{
#if 0 //QT5
#if KDEPIM_HAVE_X11
    m_toDesktop->clear();
    NETRootInfo wm_root( QX11Info::display(), NET::NumberOfDesktops |
                         NET::DesktopNames );
    NETWinInfo wm_client( QX11Info::display(), winId(),
                          QX11Info::appRootWindow(), NET::WMDesktop );

    QAction *act = m_toDesktop->addAction(i18n( "&All Desktops" ));
    if (wm_client.desktop() == NETWinInfo::OnAllDesktops) {
        act->setChecked(true);
    }
    QAction *separator = new QAction(m_toDesktop);
    separator->setSeparator(true);
    m_toDesktop->addAction(separator);
    const int count = wm_root.numberOfDesktops();
    for ( int n = 1; n <= count; ++n ) {
        QAction *desktopAct = m_toDesktop->addAction(QString::fromLatin1( "&%1 %2" ).arg( n ).arg(QString::fromUtf8( wm_root.desktopName( n ) ) ));
        if (wm_client.desktop() == n) {
            desktopAct->setChecked(true);
        }
    }
#endif
#endif
}


// -------------------- private methods -------------------- //

void KNote::buildGui()
{
    createNoteHeader();
    createNoteEditor(QString());

    KXMLGUIBuilder builder( this );
    KXMLGUIFactory factory( &builder, this );
    factory.addClient( this );

    m_menu = dynamic_cast<QMenu*>( factory.container( QLatin1String("note_context"), this ) );
    m_tool = dynamic_cast<KToolBar*>( factory.container( QLatin1String("note_tool"), this ) );

    createNoteFooter();
}

void KNote::createActions()
{
    // create the menu items for the note - not the editor...
    // rename, mail, print, save as, insert date, alarm, close, delete, new note
    QAction *action;

    action  = new QAction( QIcon::fromTheme( QLatin1String("document-new") ), i18n( "New" ),  this );
    actionCollection()->addAction( QLatin1String("new_note"), action );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotRequestNewNote()) );

    action  = new QAction( QIcon::fromTheme( QLatin1String("edit-rename") ), i18n( "Rename..." ), this );
    actionCollection()->addAction( QLatin1String("rename_note"), action );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotRename()) );

    m_readOnly  = new KToggleAction( QIcon::fromTheme( QLatin1String("object-locked") ),
                                     i18n( "Lock" ), this );
    actionCollection()->addAction( QLatin1String("lock_note"), m_readOnly );
    connect( m_readOnly, SIGNAL(triggered(bool)),
             SLOT(slotUpdateReadOnly()) );
    m_readOnly->setCheckedState( KGuiItem( i18n( "Unlock" ), QLatin1String("object-unlocked") ) );

    action  = new QAction( QIcon::fromTheme( QLatin1String("window-close") ), i18n( "Hide" ), this );
    actionCollection()->addAction( QLatin1String("hide_note"), action );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotClose()) );
    action->setShortcut( QKeySequence( Qt::Key_Escape ) );

    action  = new QAction( QIcon::fromTheme( QLatin1String("edit-delete") ), i18n( "Delete" ), this );
    actionCollection()->addAction( QLatin1String("delete_note"), action );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotKill()),Qt::QueuedConnection );

    action  = new QAction( QIcon::fromTheme( QLatin1String("knotes_alarm") ), i18n( "Set Alarm..." ),
                           this );
    actionCollection()->addAction( QLatin1String("set_alarm"), action );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotSetAlarm()) );

    action  = new QAction( QIcon::fromTheme( QLatin1String("network-wired") ), i18n( "Send..." ), this );
    actionCollection()->addAction( QLatin1String("send_note"), action );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotSend()) );

    action  = new QAction( QIcon::fromTheme( QLatin1String("mail-send") ), i18n( "Mail..." ), this );
    actionCollection()->addAction( QLatin1String("mail_note"), action );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotMail()) );

    action  = new QAction( QIcon::fromTheme( QLatin1String("document-save-as") ), i18n( "Save As..." ),
                           this );
    actionCollection()->addAction( QLatin1String("save_note"), action );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotSaveAs()) );
    actionCollection()->addAction( KStandardAction::Print,  QLatin1String("print_note"), this,
                                   SLOT(slotPrint()) );

    if (KPrintPreview::isAvailable()) {
        actionCollection()->addAction( KStandardAction::PrintPreview,  QLatin1String("print_preview_note"), this,
                                       SLOT(slotPrintPreview()) );
    }
    action  = new QAction( QIcon::fromTheme( QLatin1String("configure") ), i18n( "Preferences..." ), this );
    actionCollection()->addAction( QLatin1String("configure_note"), action );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotPreferences()) );


    m_keepAbove  = new KToggleAction( QIcon::fromTheme( QLatin1String("go-up") ),
                                      i18n( "Keep Above Others" ), this );
    actionCollection()->addAction( QLatin1String("keep_above"), m_keepAbove );
    connect( m_keepAbove, SIGNAL(triggered(bool)),
             SLOT(slotKeepAbove()) );

    m_keepBelow  = new KToggleAction( QIcon::fromTheme( QLatin1String("go-down") ),
                                      i18n( "Keep Below Others" ), this );
    actionCollection()->addAction( QLatin1String("keep_below"), m_keepBelow );
    connect( m_keepBelow, SIGNAL(triggered(bool)),
             SLOT(slotKeepBelow()) );

#if KDEPIM_HAVE_X11
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
    action  = new QAction( i18n( "Walk Through Notes" ), this );
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
    m_button = new KNoteButton( QLatin1String("knotes_close"), this );
    headerLayout->addWidget( m_button );

    connect( m_button, SIGNAL(clicked()), this, SLOT(slotClose()) );

    m_noteLayout->addItem( headerLayout );
}

void KNote::createNoteEditor(const QString &configFile)
{
    m_editor = new KNoteEdit( configFile, actionCollection(), this );
    m_noteLayout->addWidget( m_editor );
    m_editor->setNote( this );
    m_editor->installEventFilter( this ); // receive focus events for modified
    setFocusProxy( m_editor );
}

void KNote::slotRequestNewNote()
{
    //Be sure to save before to request a new note
    saveNote();
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

void KNote::loadNoteContent(const Akonadi::Item &item)
{
    KMime::Message::Ptr noteMessage = item.payload<KMime::Message::Ptr>();
    const KMime::Headers::Subject * const subject = noteMessage ? noteMessage->subject(false) : 0;
    setName(subject ? subject->asUnicodeString() : QString());
    if ( noteMessage->contentType()->isHTMLText() ) {
        m_editor->setAcceptRichText(true);
        m_editor->setHtml(noteMessage->mainBodyPart()->decodedText());
    } else {
        m_editor->setAcceptRichText(false);
        m_editor->setPlainText(noteMessage->mainBodyPart()->decodedText());
    }
    if ( noteMessage->headerByType( "X-Cursor-Position" ) ) {
        m_editor->setCursorPositionFromStart( noteMessage->headerByType( "X-Cursor-Position" )->asUnicodeString().toInt() );
    }
}

void KNote::prepare()
{
    mBlockSave = true;
    loadNoteContent(mItem);

    resize(mDisplayAttribute->size());
    const QPoint& position = mDisplayAttribute->position();
    QRect desk = kapp->desktop()->rect();
    desk.adjust( 10, 10, -10, -10 );
    if ( desk.intersects( QRect( position, mDisplayAttribute->size() ) ) ) {
        move( position );           // do before calling show() to avoid flicker
    }
    if (mDisplayAttribute->isHidden())
        hide();
    else
        show();
    // read configuration settings...
    slotApplyConfig();

    if ( mItem.hasAttribute<NoteShared::NoteLockAttribute>() ) {
        m_editor->setReadOnly(true);
        m_readOnly->setChecked( true );
    } else {
        m_readOnly->setChecked( false );
    }
    slotUpdateReadOnly();
    // if this is a new note put on current desktop - we can't use defaults
    // in KConfig XT since only _changes_ will be stored in the config file
    int desktop = mDisplayAttribute->desktop();

#if KDEPIM_HAVE_X11
    if ( ( desktop < 0 && desktop != NETWinInfo::OnAllDesktops ) ||
         !mDisplayAttribute->rememberDesktop() )
        desktop = KWindowSystem::currentDesktop();
#endif

    // show the note if desired
    if ( desktop != 0 && !mDisplayAttribute->isHidden() ) {
        // to avoid flicker, call this before show()
        toDesktop( desktop );
        show();

        // because KWin forgets about that for hidden windows
#if KDEPIM_HAVE_X11
        if ( desktop == NETWinInfo::OnAllDesktops ) {
            toDesktop( desktop );
        }
#endif
    }

    if ( mDisplayAttribute->keepAbove() ) {
        m_keepAbove->setChecked( true );
    } else if ( mDisplayAttribute->keepBelow() ) {
        m_keepBelow->setChecked( true );
    } else {
        m_keepAbove->setChecked( false );
        m_keepBelow->setChecked( false );
    }

    slotUpdateKeepAboveBelow();
    // HACK: update the icon color - again after showing the note, to make kicker
    // aware of the new colors
    KIconEffect effect;
    const QColor col = mDisplayAttribute->backgroundColor();
    const QPixmap icon = effect.apply( qApp->windowIcon().pixmap(
                                     IconSize( KIconLoader::Desktop ),
                                     IconSize( KIconLoader::Desktop ) ),
                                 KIconEffect::Colorize,
                                 1, col, false );
#if KDEPIM_HAVE_X11
    const QPixmap miniIcon = effect.apply( qApp->windowIcon().pixmap(
                                         IconSize( KIconLoader::Small ),
                                         IconSize( KIconLoader::Small ) ),
                                     KIconEffect::Colorize,
                                     1, col, false );
    KWindowSystem::setIcons( winId(), icon, miniIcon );
#endif

    // set up the look&feel of the note
    setFrameStyle( Panel | Raised );
    setMinimumSize( 20, 20 );
    setBackgroundRole( QPalette::Base );

    m_editor->setContentsMargins( 0, 0, 0, 0 );
    m_editor->setBackgroundRole( QPalette::Base );
    m_editor->setFrameStyle( NoFrame );
    m_editor->document()->setModified( false );
    mBlockSave = false;
}

void KNote::toDesktop( int desktop )
{
    if ( desktop == 0 ) {
        return;
    }

#if KDEPIM_HAVE_X11
    if ( desktop == NETWinInfo::OnAllDesktops ) {
        KWindowSystem::setOnAllDesktops( winId(), true );
    } else {
        KWindowSystem::setOnDesktop( winId(), desktop );
    }
#endif
}

void KNote::setColor( const QColor &fg, const QColor &bg )
{
    m_editor->setColor(fg,bg);
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
#if KDEPIM_HAVE_X11
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
    if ( m_label->fontMetrics().boundingRect( labelText ).width() > m_label->width() ) {
        m_label->setAlignment( Qt::AlignLeft );
    } else {
        m_label->setAlignment( Qt::AlignHCenter );
    }
}

void KNote::updateFocus()
{
    if ( hasFocus() ) {
        if ( !m_editor->isReadOnly() ) {
            if ( m_tool && m_tool->isHidden() && m_editor->acceptRichText() ) {
                m_tool->show();
                updateLayout();
            }
            m_grip->show();
        } else {
            if ( m_tool && !m_tool->isHidden() ) {
                m_tool->hide();
                updateLayout();     // to update the minimum height
            }
            m_grip->hide();
        }
    } else {
        m_grip->hide();

        if ( m_tool && !m_tool->isHidden() ) {
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
    if ( mDisplayAttribute->isHidden() ) {
        // KWin does not preserve these properties for hidden windows
        slotUpdateKeepAboveBelow(false);
        slotUpdateShowInTaskbar();
        toDesktop( mDisplayAttribute->desktop() );
        move( mDisplayAttribute->position() );
        NoteShared::NoteDisplayAttribute *attr =  mItem.attribute<NoteShared::NoteDisplayAttribute>( Akonadi::Entity::AddIfMissing );
        saveNoteContent();
        attr->setIsHidden(false);
        if (!mBlockSave) {
            Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
            qDebug()<<"showEvent slotNoteSaved(KJob*)";
#endif
            connect( job, SIGNAL(result(KJob*)), SLOT(slotNoteSaved(KJob*)) );
        }
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
    if ( !m_editor->isReadOnly() ) {
        e->setAccepted( e->mimeData()->hasColor() );
    }
}

void KNote::dropEvent( QDropEvent *e )
{
    if ( m_editor->isReadOnly() ) {
        return;
    }

    const QMimeData *md = e->mimeData();
    if ( md->hasColor() ) {
        const QColor bg =  qvariant_cast<QColor>( md->colorData() );

        NoteShared::NoteDisplayAttribute *attr =  mItem.attribute<NoteShared::NoteDisplayAttribute>( Akonadi::Entity::AddIfMissing );
        saveNoteContent();
        attr->setBackgroundColor(bg);
        Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
        qDebug()<<"dropEvent slotNoteSaved(KJob*)";
#endif
        connect( job, SIGNAL(result(KJob*)), SLOT(slotNoteSaved(KJob*)) );
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
#if 0 //QT5
#if KDEPIM_HAVE_X11
            e->button() == Qt::LeftButton ? KWindowSystem::raiseWindow( winId() )
                                          : KWindowSystem::lowerWindow( winId() );

            XUngrabPointer( QX11Info::display(), QX11Info::appTime() );
            NETRootInfo wm_root( QX11Info::display(), NET::WMMoveResize );
            wm_root.moveResizeRequest( winId(), e->globalX(), e->globalY(),
                                       NET::Move );
#endif
#endif
            return true;
        }

        if ( ev->type() == QEvent::MouseButtonRelease ) {
#if 0 //QT5
#if KDEPIM_HAVE_X11
            NETRootInfo wm_root( QX11Info::display(), NET::WMMoveResize );
            wm_root.moveResizeRequest( winId(), e->globalX(), e->globalY(),
                                       NET::MoveResizeCancel );
#endif
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
                if (!mBlockSave)
                    saveNote(true);
            }
        } else if ( ev->type() == QEvent::FocusIn ) {
            updateFocus();
        }

        return false;
    }

    return false;
}

Akonadi::Item KNote::item() const
{
    return mItem;
}
