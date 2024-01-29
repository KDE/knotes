/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2013 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include "knote.h"
#include "alarms/notealarmdialog.h"
#include "attributes/notealarmattribute.h"
#include "attributes/notedisplayattribute.h"
#include "attributes/notelockattribute.h"
#include "configdialog/knotesimpleconfigdialog.h"
#include "knotedisplaysettings.h"
#include "knoteedit.h"
#include "knotes_debug.h"
#include "knotesglobalconfig.h"
#include "notes/knotebutton.h"
#include "noteutils.h"
#include "print/knoteprinter.h"
#include "print/knoteprintobject.h"
#include "print/knoteprintselectthemedialog.h"
#include "utils/knoteutils.h"

#include <Akonadi/ItemModifyJob>

#include <Debug/akonadisearchdebugdialog.h>

#include <KMime/KMimeMessage>

#include <KActionCollection>
#include <KComboBox>
#include <KFileCustomDialog>
#include <KIconEffect>
#include <KLocalizedString>
#include <KMessageBox>
#include <KToggleAction>
#include <KToolBar>
#include <KWindowSystem>
#include <KXMLGUIBuilder>
#include <KXMLGUIFactory>

#include <QApplication>
#include <QCheckBox>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMimeData>
#include <QPointer>
#include <QScreen>
#include <QSizeGrip>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWindow>

#if KDEPIM_HAVE_X11
#include <KWindowInfo>
#include <KX11Extras>
#include <NETWM>
#include <fixx11h.h>
#endif

//#define DEBUG_SAVE_NOTE 1

KNote::KNote(const QDomDocument &buildDoc, const Akonadi::Item &item, bool allowAkonadiSearchDebug, QWidget *parent)
    : QFrame(parent, Qt::FramelessWindowHint)
    , mItem(item)
    , m_kwinConf(KSharedConfig::openConfig(QStringLiteral("kwinrc")))
    , mDisplayAttribute(new KNoteDisplaySettings)
    , mAllowDebugAkonadiSearch(allowAkonadiSearchDebug)
{
    if (mItem.hasAttribute<NoteShared::NoteDisplayAttribute>()) {
        mDisplayAttribute->setDisplayAttribute(mItem.attribute<NoteShared::NoteDisplayAttribute>());
    } else {
        setDisplayDefaultValue();
        // save default display value
    }
    setAcceptDrops(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setDOMDocument(buildDoc);
    setXMLFile(componentName() + QLatin1StringView("ui.rc"), false, false);

    // create the main layout
    m_noteLayout = new QVBoxLayout(this);
    m_noteLayout->setContentsMargins(0, 0, 0, 0);
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
    auto job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
    qCDebug(KNOTES_LOG) << "setDisplayDefaultValue slotNoteSaved(KJob*)";
#endif
    connect(job, &Akonadi::ItemModifyJob::result, this, &KNote::slotNoteSaved);
}

void KNote::setChangeItem(const Akonadi::Item &item, const QSet<QByteArray> &set)
{
    mItem = item;
    if (item.hasAttribute<NoteShared::NoteDisplayAttribute>()) {
        mDisplayAttribute->setDisplayAttribute(item.attribute<NoteShared::NoteDisplayAttribute>());
    }
    if (set.contains("KJotsLockAttribute")) {
        m_editor->setReadOnly(item.hasAttribute<NoteShared::NoteLockAttribute>());
    }
    if (set.contains("PLD:RFC822")) {
        loadNoteContent(item);
    }
    if (set.contains("NoteDisplayAttribute")) {
        qCDebug(KNOTES_LOG) << " ATR:NoteDisplayAttribute";
        slotApplyConfig();
    }
    // TODO update display/content etc.
    updateLabelAlignment();
}

void KNote::slotKill(bool force)
{
    if (!force
        && KMessageBox::warningContinueCancel(this,
                                              i18n("<qt>Do you really want to delete note <b>%1</b>?</qt>", m_label->text()),
                                              i18n("Confirm Delete"),
                                              KGuiItem(i18n("&Delete"), QStringLiteral("edit-delete")),
                                              KStandardGuiItem::cancel(),
                                              QStringLiteral("ConfirmDeleteNote"))
            != KMessageBox::Continue) {
        return;
    }

    Q_EMIT sigKillNote(mItem.id());
}

// -------------------- public member functions -------------------- //

void KNote::saveNote(bool force, bool sync)
{
    if (!force && !m_editor->document()->isModified()) {
        return;
    }
    bool needToSave = false;
    auto attribute = mItem.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Item::AddIfMissing);
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
    KWindowInfo info(winId(), NET::WMDesktop);
    const int count = KX11Extras::numberOfDesktops();
    for (int n = 1; n <= count; ++n) {
        if (info.isOnDesktop(n)) {
            if (attribute->desktop() != n) {
                needToSave = true;
                attribute->setDesktop(n);
                break;
            }
        }
    }
#endif
    if (m_editor->document()->isModified()) {
        needToSave = true;
        saveNoteContent();
    }
    if (needToSave) {
#ifdef DEBUG_SAVE_NOTE
        qCDebug(KNOTES_LOG) << "save Note slotClose() slotNoteSaved(KJob*) : sync" << sync;
#endif
        auto job = new Akonadi::ItemModifyJob(mItem);
        if (sync) {
            job->exec();
        } else {
#ifdef DEBUG_SAVE_NOTE
            qCDebug(KNOTES_LOG) << "save Note slotClose() slotNoteSaved(KJob*)";
#endif
            connect(job, &Akonadi::ItemModifyJob::result, this, &KNote::slotNoteSaved);
        }
    }
}

void KNote::slotNoteSaved(KJob *job)
{
    qCDebug(KNOTES_LOG) << " void KNote::slotNoteSaved(KJob *job)";
    if (job->error()) {
        qCDebug(KNOTES_LOG) << " problem during save note:" << job->errorString();
    } else {
        m_editor->document()->setModified(false);
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

void KNote::setName(const QString &name)
{
    m_label->setText(name);
    updateLabelAlignment();

    if (m_editor) { // not called from CTOR?
        saveNote();
    }
    setWindowTitle(name);

    Q_EMIT sigNameChanged(name);
}

void KNote::setText(const QString &text)
{
    m_editor->setText(text);

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
    const QString newName = QInputDialog::getText(this, QString(), i18n("Please enter the new name:"), QLineEdit::Normal, m_label->text(), &ok);
    if (!ok || (oldName == newName)) { // handle cancel
        return;
    }

    setName(newName);
}

void KNote::slotUpdateReadOnly()
{
    const bool readOnly = m_readOnly->isChecked();

    m_editor->setReadOnly(readOnly);

    if (mItem.hasAttribute<NoteShared::NoteLockAttribute>()) {
        if (!readOnly) {
            mItem.removeAttribute<NoteShared::NoteLockAttribute>();
        }
    } else {
        if (readOnly) {
            mItem.attribute<NoteShared::NoteLockAttribute>(Akonadi::Item::AddIfMissing);
        }
    }
    if (!mBlockSave) {
        updateAllAttributes();
        auto job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
        qCDebug(KNOTES_LOG) << " void KNote::slotUpdateReadOnly() slotNoteSaved(KJob*)";
#endif
        connect(job, &Akonadi::ItemModifyJob::result, this, &KNote::slotNoteSaved);
    }

    // enable/disable actions accordingly
    actionCollection()->action(QStringLiteral("configure_note"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("delete_note"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_bold"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_italic"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_underline"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_strikeout"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_alignleft"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_aligncenter"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_alignright"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_alignblock"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_list"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_super"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_sub"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_increaseindent"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_decreaseindent"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("text_background_color"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_size"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("format_color"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("rename_note"))->setEnabled(!readOnly);
    actionCollection()->action(QStringLiteral("set_alarm"))->setEnabled(!readOnly);
    m_keepAbove->setEnabled(!readOnly);
    m_keepBelow->setEnabled(!readOnly);

#if KDEPIM_HAVE_X11
    m_toDesktop->setEnabled(!readOnly);
#endif

    updateFocus();
}

void KNote::updateAllAttributes()
{
    auto attribute = mItem.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Item::AddIfMissing);
#if KDEPIM_HAVE_X11
    KWindowInfo info(winId(), NET::WMDesktop);
    const int count = KX11Extras::numberOfDesktops();
    for (int n = 1; n <= count; ++n) {
        if (info.isOnDesktop(n)) {
            attribute->setDesktop(n);
        }
    }
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
    auto job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
    qCDebug(KNOTES_LOG) << "slotClose() slotNoteSaved(KJob*)";
#endif
    connect(job, &Akonadi::ItemModifyJob::result, this, &KNote::slotNoteSaved);
    hide();
}

void KNote::slotSetAlarm()
{
    QPointer<NoteShared::NoteAlarmDialog> dlg = new NoteShared::NoteAlarmDialog(name(), this);
    if (mItem.hasAttribute<NoteShared::NoteAlarmAttribute>()) {
        dlg->setAlarm(mItem.attribute<NoteShared::NoteAlarmAttribute>()->dateTime());
    }
    if (dlg->exec()) {
        bool needToModify = true;
        QDateTime dateTime = dlg->alarm();
        if (dateTime.isValid()) {
            auto attribute = mItem.attribute<NoteShared::NoteAlarmAttribute>(Akonadi::Item::AddIfMissing);
            attribute->setDateTime(dateTime);
        } else {
            if (mItem.hasAttribute<NoteShared::NoteAlarmAttribute>()) {
                mItem.removeAttribute<NoteShared::NoteAlarmAttribute>();
            } else {
                needToModify = false;
            }
        }
        if (needToModify) {
            // Verify it!
            saveNoteContent();
            auto job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
            qCDebug(KNOTES_LOG) << "setAlarm() slotNoteSaved(KJob*)";
#endif
            connect(job, &Akonadi::ItemModifyJob::result, this, &KNote::slotNoteSaved);
        }
    }
    delete dlg;
}

void KNote::saveNoteContent()
{
    auto message = mItem.payload<KMime::Message::Ptr>();
    const QByteArray encoding("utf-8");
    message->subject(true)->fromUnicodeString(name(), encoding);
    message->contentType(true)->setMimeType(m_editor->acceptRichText() ? "text/html" : "text/plain");
    message->contentType()->setCharset(encoding);
    message->contentTransferEncoding(true)->setEncoding(KMime::Headers::CEquPr);
    message->date(true)->setDateTime(QDateTime::currentDateTime());
    message->mainBodyPart()->fromUnicodeString(text().isEmpty() ? QStringLiteral(" ") : text());

    auto header = new KMime::Headers::Generic("X-Cursor-Position");
    header->fromUnicodeString(QString::number(m_editor->cursorPositionFromStart()), "utf-8");
    message->setHeader(header);

    message->assemble();

    mItem.setPayload(message);
}

void KNote::slotPreferences()
{
    // create a new preferences dialog...
    QPointer<KNoteSimpleConfigDialog> dialog = new KNoteSimpleConfigDialog(name(), this);
    auto attribute = mItem.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Item::AddIfMissing);
    attribute->setSize(QSize(width(), height()));

    dialog->load(mItem, m_editor->acceptRichText());
    connect(this, &KNote::sigNameChanged, dialog.data(), &KNoteSimpleConfigDialog::slotUpdateCaption);
    if (dialog->exec()) {
        bool isRichText;
        dialog->save(mItem, isRichText);
        m_editor->setAcceptRichText(isRichText);
        saveNoteContent();
        auto job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
        qCDebug(KNOTES_LOG) << "slotPreference slotNoteSaved(KJob*)";
#endif
        connect(job, &Akonadi::ItemModifyJob::result, this, &KNote::slotNoteSaved);
    }
    delete dialog;
}

void KNote::slotSend()
{
    NoteShared::NoteUtils noteUtils;
    noteUtils.sendToNetwork(this, name(), text());
}

void KNote::slotMail()
{
    NoteShared::NoteUtils noteUtils;
    noteUtils.sendToMail(this, m_label->text(), m_editor->toPlainText());
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
    if (isModified()) {
        saveNote();
    }

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
        KNotePrinter printer(this);
        QList<KNotePrintObject *> lst;
        lst.append(new KNotePrintObject(mItem));
        printer.setDefaultFont(mDisplayAttribute->font());
        printer.printNotes(lst, printingTheme, preview);
        qDeleteAll(lst);
    }
}

void KNote::slotSaveAs()
{
    // TODO: where to put pdf file support? In the printer??!??!
    QUrl url;
    QPointer<KFileCustomDialog> dlg = new KFileCustomDialog(this);
    QCheckBox *convert = nullptr;
    if (m_editor->acceptRichText()) {
        convert = new QCheckBox(dlg.data());
        convert->setText(i18n("Save note as plain text"));
    }
    if (convert) {
        dlg->setCustomWidget(convert);
    }
    dlg->setUrl(url);
    dlg->setOperationMode(KFileWidget::Saving);
    dlg->setWindowTitle(i18nc("@title:window", "Save As"));
    if (!dlg->exec()) {
        delete dlg;
        return;
    }

    const QString fileName = dlg->fileWidget()->selectedFile();
    const bool htmlFormatAndSaveAsHtml = (convert && !convert->isChecked());
    delete dlg;
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (file.exists()
        && KMessageBox::warningContinueCancel(this,
                                              i18n("<qt>A file named <b>%1</b> already exists.<br />"
                                                   "Are you sure you want to overwrite it?</qt>",
                                                   QFileInfo(file).fileName()))
            != KMessageBox::Continue) {
        return;
    }

    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        if (htmlFormatAndSaveAsHtml) {
            QString htmlStr = m_editor->toHtml();
            htmlStr.replace(QStringLiteral("meta name=\"qrichtext\" content=\"1\""),
                            QStringLiteral("meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\""));
            stream << htmlStr;
        } else {
            stream << m_editor->toPlainText();
        }
    }
}

void KNote::slotPopupActionToDesktop(QAction *act)
{
    const int id = act->data().toInt();
    toDesktop(id); // compensate for the menu separator, -1 == all desktops
}

// ------------------ private slots (configuration) ------------------ //

void KNote::slotApplyConfig()
{
    m_label->setFont(mDisplayAttribute->titleFont());
    m_editor->setTextFont(mDisplayAttribute->font());
    m_editor->setTabStop(mDisplayAttribute->tabSize());
    m_editor->setAutoIndentMode(mDisplayAttribute->autoIndent());

    setColor(mDisplayAttribute->foregroundColor(), mDisplayAttribute->backgroundColor());

    updateLayout();
#if KDEPIM_HAVE_X11
    slotUpdateShowInTaskbar();
#endif
    resize(mDisplayAttribute->size());
}

void KNote::slotKeepAbove()
{
    if (m_keepBelow->isChecked()) {
        m_keepBelow->setChecked(false);
    }
    updateKeepAboveBelow();
}

void KNote::slotKeepBelow()
{
    if (m_keepAbove->isChecked()) {
        m_keepAbove->setChecked(false);
    }
    updateKeepAboveBelow();
}

void KNote::updateKeepAboveBelow(bool save)
{
    auto attribute = mItem.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Item::AddIfMissing);
    if (m_keepAbove->isChecked()) {
        attribute->setKeepAbove(true);
        attribute->setKeepBelow(false);
        windowHandle()->setFlag(Qt::WindowStaysOnTopHint, true);
    } else if (m_keepBelow->isChecked()) {
        attribute->setKeepAbove(false);
        attribute->setKeepBelow(true);
        windowHandle()->setFlag(Qt::WindowStaysOnBottomHint, true);
    } else {
        attribute->setKeepAbove(false);
        attribute->setKeepBelow(false);
        windowHandle()->setFlag(Qt::WindowStaysOnTopHint, false);
        windowHandle()->setFlag(Qt::WindowStaysOnBottomHint, false);
    }
    if (!mBlockSave && save) {
        saveNoteContent();
        auto job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
        qCDebug(KNOTES_LOG) << "slotUpdateKeepAboveBelow slotNoteSaved(KJob*)";
#endif
        connect(job, &Akonadi::ItemModifyJob::result, this, &KNote::slotNoteSaved);
    }
}

void KNote::slotUpdateShowInTaskbar()
{
#if KDEPIM_HAVE_X11
    if (KWindowSystem::isPlatformX11()) {
        if (!mDisplayAttribute->showInTaskbar()) {
            KX11Extras::setState(winId(), KWindowInfo(winId(), NET::WMState).state() | NET::SkipTaskbar);
        } else {
            KX11Extras::clearState(winId(), NET::SkipTaskbar);
        }
    }
#endif
}

void KNote::slotUpdateDesktopActions()
{
#if KDEPIM_HAVE_X11
    m_toDesktop->clear();

    QAction *act = m_toDesktop->addAction(i18n("&All Desktops"));
    KWindowInfo info(winId(), NET::WMDesktop);

    if (info.onAllDesktops()) {
        act->setChecked(true);
        act->setData(NETWinInfo::OnAllDesktops);
    }
    auto separator = new QAction(m_toDesktop);
    separator->setSeparator(true);
    m_toDesktop->addAction(separator);
    const int count = KX11Extras::numberOfDesktops();
    for (int n = 1; n <= count; ++n) {
        QAction *desktopAct = m_toDesktop->addAction(QStringLiteral("&%1 %2").arg(n).arg(KX11Extras::desktopName(n)));
        desktopAct->setData(n);
        if (info.isOnDesktop(n)) {
            desktopAct->setChecked(true);
        }
    }
#endif
}

// -------------------- private methods -------------------- //

void KNote::buildGui()
{
    createNoteHeader();
    createNoteEditor(QString());

    KXMLGUIBuilder builder(this);
    KXMLGUIFactory factory(&builder, this);
    factory.addClient(this);

    m_menu = qobject_cast<QMenu *>(factory.container(QStringLiteral("note_context"), this));
    m_tool = qobject_cast<KToolBar *>(factory.container(QStringLiteral("note_tool"), this));

    createNoteFooter();
}

void KNote::createActions()
{
    // create the menu items for the note - not the editor...
    // rename, mail, print, save as, insert date, alarm, close, delete, new note
    auto action = new QAction(QIcon::fromTheme(QStringLiteral("document-new")), i18n("New"), this);
    actionCollection()->addAction(QStringLiteral("new_note"), action);
    connect(action, &QAction::triggered, this, &KNote::slotRequestNewNote);

    action = new QAction(QIcon::fromTheme(QStringLiteral("edit-rename")), i18n("Rename..."), this);
    actionCollection()->addAction(QStringLiteral("rename_note"), action);
    connect(action, &QAction::triggered, this, &KNote::slotRename);

    m_readOnly = new KToggleAction(QIcon::fromTheme(QStringLiteral("object-locked")), i18n("Lock"), this);
    actionCollection()->addAction(QStringLiteral("lock_note"), m_readOnly);
    connect(m_readOnly, &KToggleAction::triggered, this, &KNote::slotUpdateReadOnly);
    m_readOnly->setCheckedState(KGuiItem(i18n("Unlock"), QStringLiteral("object-unlocked")));

    action = new QAction(QIcon::fromTheme(QStringLiteral("window-close")), i18n("Hide"), this);
    actionCollection()->addAction(QStringLiteral("hide_note"), action);
    connect(action, &QAction::triggered, this, &KNote::slotClose);
    actionCollection()->setDefaultShortcut(action, QKeySequence(Qt::Key_Escape));

    action = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Delete"), this);
    actionCollection()->addAction(QStringLiteral("delete_note"), action);
    connect(action, &QAction::triggered, this, &KNote::slotKill);

    action = new QAction(QIcon::fromTheme(QStringLiteral("knotes_alarm")), i18n("Set Alarm..."), this);
    actionCollection()->addAction(QStringLiteral("set_alarm"), action);
    connect(action, &QAction::triggered, this, &KNote::slotSetAlarm);

    action = new QAction(QIcon::fromTheme(QStringLiteral("network-wired")), i18n("Send..."), this);
    actionCollection()->addAction(QStringLiteral("send_note"), action);
    connect(action, &QAction::triggered, this, &KNote::slotSend);

    action = new QAction(QIcon::fromTheme(QStringLiteral("mail-send")), i18n("Mail..."), this);
    actionCollection()->addAction(QStringLiteral("mail_note"), action);
    connect(action, &QAction::triggered, this, &KNote::slotMail);

    action = new QAction(QIcon::fromTheme(QStringLiteral("document-save-as")), i18n("Save As..."), this);
    actionCollection()->addAction(QStringLiteral("save_note"), action);
    connect(action, &QAction::triggered, this, &KNote::slotSaveAs);
    action = actionCollection()->addAction(KStandardAction::Print, QStringLiteral("print_note"));
    connect(action, &QAction::triggered, this, &KNote::slotPrint);

    action = actionCollection()->addAction(KStandardAction::PrintPreview, QStringLiteral("print_preview_note"));
    connect(action, &QAction::triggered, this, &KNote::slotPrintPreview);

    action = new QAction(QIcon::fromTheme(QStringLiteral("configure")), i18n("Preferences..."), this);
    actionCollection()->addAction(QStringLiteral("configure_note"), action);
    connect(action, &QAction::triggered, this, &KNote::slotPreferences);

    m_keepAbove = new KToggleAction(QIcon::fromTheme(QStringLiteral("go-up")), i18n("Keep Above Others"), this);
    actionCollection()->addAction(QStringLiteral("keep_above"), m_keepAbove);
    connect(m_keepAbove, &KToggleAction::triggered, this, &KNote::slotKeepAbove);

    m_keepBelow = new KToggleAction(QIcon::fromTheme(QStringLiteral("go-down")), i18n("Keep Below Others"), this);
    actionCollection()->addAction(QStringLiteral("keep_below"), m_keepBelow);
    connect(m_keepBelow, &KToggleAction::triggered, this, &KNote::slotKeepBelow);

#if KDEPIM_HAVE_X11
    m_toDesktop = new KSelectAction(i18n("To Desktop"), this);
    actionCollection()->addAction(QStringLiteral("to_desktop"), m_toDesktop);
    connect(m_toDesktop, &KSelectAction::actionTriggered, this, &KNote::slotPopupActionToDesktop);
    connect(m_toDesktop->menu(), &QMenu::aboutToShow, this, &KNote::slotUpdateDesktopActions);
    // initially populate it, otherwise stays disabled
    slotUpdateDesktopActions();
#endif
    // invisible action to walk through the notes to make this configurable
    action = new QAction(i18n("Walk Through Notes"), this);
    actionCollection()->addAction(QStringLiteral("walk_notes"), action);
    connect(action, &QAction::triggered, this, &KNote::sigShowNextNote);
    actionCollection()->setDefaultShortcut(action, QKeySequence(Qt::SHIFT | Qt::Key_Backtab));

    actionCollection()->addAssociatedWidget(this);
    const auto lst = actionCollection()->actions();
    for (QAction *act : lst) {
        act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    }
    if (mAllowDebugAkonadiSearch) {
        // Don't translate it it's just for debugging
        action = new QAction(QStringLiteral("Debug Akonadi Search..."), this);
        actionCollection()->addAction(QStringLiteral("debug_akonadi_search"), action);
        connect(action, &QAction::triggered, this, &KNote::slotDebugAkonadiSearch);
    }
}

void KNote::createNoteHeader()
{
    // load style configuration
    KConfigGroup styleGroup(m_kwinConf, QStringLiteral("Style"));

    QBoxLayout::Direction headerLayoutDirection = QBoxLayout::LeftToRight;

    if (styleGroup.readEntry("CustomButtonPositions", false)) {
        if (styleGroup.readEntry("ButtonsOnLeft").contains(QLatin1Char('X'))) {
            headerLayoutDirection = QBoxLayout::RightToLeft;
        }
    }

    auto headerLayout = new QBoxLayout(headerLayoutDirection);

    // create header label
    m_label = new QLabel(this);
    headerLayout->addWidget(m_label);
    m_label->setFrameStyle(NoFrame);
    m_label->setBackgroundRole(QPalette::Base);
    m_label->setLineWidth(0);
    m_label->setAutoFillBackground(true);
    m_label->installEventFilter(this); // receive events ( for dragging &
    // action menu )
    m_button = new KNoteButton(QStringLiteral("knotes_close"), this);
    headerLayout->addWidget(m_button);

    connect(m_button, &KNoteButton::clicked, this, &KNote::slotClose);

    m_noteLayout->addLayout(headerLayout);
}

void KNote::createNoteEditor(const QString &configFile)
{
    Q_UNUSED(configFile)
    m_editor = new KNoteEdit(actionCollection(), this);
    m_noteLayout->addWidget(m_editor);
    m_editor->setNote(this);
    m_editor->installEventFilter(this); // receive focus events for modified
    setFocusProxy(m_editor);
}

void KNote::slotRequestNewNote()
{
    // Be sure to save before to request a new note
    saveNote();
    Q_EMIT sigRequestNewNote();
}

void KNote::createNoteFooter()
{
    if (m_tool) {
        m_tool->setIconSize(QSize(10, 10));
        m_tool->setFixedHeight(24);
        m_tool->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    // create size grip
    auto gripLayout = new QHBoxLayout;
    m_grip = new QSizeGrip(this);
    m_grip->setFixedSize(m_grip->sizeHint());

    if (m_tool) {
        gripLayout->addWidget(m_tool);
        gripLayout->setAlignment(m_tool, Qt::AlignBottom | Qt::AlignLeft);
        m_tool->hide();
    }

    gripLayout->addWidget(m_grip);
    gripLayout->setAlignment(m_grip, Qt::AlignBottom | Qt::AlignRight);
    m_noteLayout->addLayout(gripLayout);

    // if there was just a way of making KComboBox adhere the toolbar height...
    if (m_tool) {
        const auto comboboxs = m_tool->findChildren<KComboBox *>();
        for (KComboBox *combo : comboboxs) {
            QFont font = combo->font();
            font.setPointSize(7);
            combo->setFont(font);
            combo->setFixedHeight(14);
        }
    }
}

void KNote::loadNoteContent(const Akonadi::Item &item)
{
    auto noteMessage = item.payload<KMime::Message::Ptr>();
    const KMime::Headers::Subject *const subject = noteMessage ? noteMessage->subject(false) : nullptr;
    setName(subject ? subject->asUnicodeString() : QString());
    if (noteMessage->contentType()->isHTMLText()) {
        m_editor->setAcceptRichText(true);
        m_editor->setAutoFormatting(QTextEdit::AutoAll);
        m_editor->setHtml(noteMessage->mainBodyPart()->decodedText());
    } else {
        m_editor->setAcceptRichText(false);
        m_editor->setAutoFormatting(QTextEdit::AutoNone);
        m_editor->setPlainText(noteMessage->mainBodyPart()->decodedText());
    }
    if (auto hrd = noteMessage->headerByType("X-Cursor-Position")) {
        m_editor->setCursorPositionFromStart(hrd->asUnicodeString().toInt());
    }
}

void KNote::prepare()
{
    mBlockSave = true;
    loadNoteContent(mItem);

    resize(mDisplayAttribute->size());
    const QPoint &position = mDisplayAttribute->position();
    QRect desk = qApp->primaryScreen()->virtualGeometry();
    desk.adjust(10, 10, -10, -10);
    if (desk.intersects(QRect(position, mDisplayAttribute->size()))) {
        move(position); // do before calling show() to avoid flicker
    }
    if (mDisplayAttribute->isHidden()) {
        hide();
    } else {
        show();
    }
    // read configuration settings...
    slotApplyConfig();

    if (mItem.hasAttribute<NoteShared::NoteLockAttribute>()) {
        m_editor->setReadOnly(true);
        m_readOnly->setChecked(true);
    } else {
        m_readOnly->setChecked(false);
    }
    slotUpdateReadOnly();
    // if this is a new note put on current desktop - we can't use defaults
    // in KConfig XT since only _changes_ will be stored in the config file
    int desktop = mDisplayAttribute->desktop();

#if KDEPIM_HAVE_X11
    if ((desktop < 0 && desktop != NETWinInfo::OnAllDesktops) || !mDisplayAttribute->rememberDesktop()) {
        desktop = KX11Extras::currentDesktop();
    }
#endif

    // show the note if desired
    if (desktop != 0 && !mDisplayAttribute->isHidden()) {
        // to avoid flicker, call this before show()
        toDesktop(desktop);
        show();

        // because KWin forgets about that for hidden windows
#if KDEPIM_HAVE_X11
        if (desktop == NETWinInfo::OnAllDesktops) {
            toDesktop(desktop);
        }
#endif
    }

    if (mDisplayAttribute->keepAbove()) {
        m_keepAbove->setChecked(true);
    } else if (mDisplayAttribute->keepBelow()) {
        m_keepBelow->setChecked(true);
    } else {
        m_keepAbove->setChecked(false);
        m_keepBelow->setChecked(false);
    }

    updateKeepAboveBelow();

    // set up the look&feel of the note
    setFrameStyle(Panel | Raised);
    setMinimumSize(20, 20);
    setBackgroundRole(QPalette::Base);

    m_editor->setContentsMargins(0, 0, 0, 0);
    m_editor->setBackgroundRole(QPalette::Base);
    m_editor->setFrameStyle(NoFrame);
    m_editor->document()->setModified(false);
    mBlockSave = false;
}

void KNote::toDesktop(int desktop)
{
    if (desktop == 0) {
        return;
    }

#if KDEPIM_HAVE_X11
    if (desktop == NETWinInfo::OnAllDesktops) {
        KX11Extras::setOnAllDesktops(winId(), true);
    } else {
        KX11Extras::setOnDesktop(winId(), desktop);
    }
#endif
}

void KNote::setColor(const QColor &fg, const QColor &bg)
{
    m_editor->setColor(fg, bg);
    QPalette p = palette();

    // better: from light(150) to light(100) to light(75)
    // QLinearGradient g( width()/2, 0, width()/2, height() );
    // g.setColorAt( 0, bg );
    // g.setColorAt( 1, bg.darker(150) );

    p.setColor(QPalette::Window, bg);
    // p.setBrush( QPalette::Window,     g );
    p.setColor(QPalette::Base, bg);
    // p.setBrush( QPalette::Base,       g );

    p.setColor(QPalette::WindowText, fg);
    p.setColor(QPalette::Text, fg);

    p.setColor(QPalette::Button, bg.darker(116));
    p.setColor(QPalette::ButtonText, fg);

    // p.setColor( QPalette::Highlight,  bg );
    // p.setColor( QPalette::HighlightedText, fg );

    // order: Light, Midlight, Button, Mid, Dark, Shadow

    // the shadow
    p.setColor(QPalette::Light, bg.lighter(180));
    p.setColor(QPalette::Midlight, bg.lighter(150));
    p.setColor(QPalette::Mid, bg.lighter(150));
    p.setColor(QPalette::Dark, bg.darker(108));
    p.setColor(QPalette::Shadow, bg.darker(116));

    setPalette(p);

    // darker values for the active label
    p.setColor(QPalette::Active, QPalette::Base, bg.darker(116));

    m_label->setPalette(p);

    // set the text color
    m_editor->setTextColor(fg);

    // update the color of the title
    updateFocus();
    Q_EMIT sigColorChanged();
}

void KNote::updateLabelAlignment()
{
    // if the name is too long to fit, left-align it, otherwise center it (#59028)
    const QString labelText = m_label->text();
    if (m_label->fontMetrics().boundingRect(labelText).width() > m_label->width()) {
        m_label->setAlignment(Qt::AlignLeft);
    } else {
        m_label->setAlignment(Qt::AlignHCenter);
    }
}

void KNote::updateFocus()
{
    if (hasFocus()) {
        if (!m_editor->isReadOnly()) {
            if (m_tool && m_tool->isHidden() && m_editor->acceptRichText()) {
                m_tool->show();
                updateLayout();
            }
            m_grip->show();
        } else {
            if (m_tool && !m_tool->isHidden()) {
                m_tool->hide();
                updateLayout(); // to update the minimum height
            }
            m_grip->hide();
        }
    } else {
        m_grip->hide();

        if (m_tool && !m_tool->isHidden()) {
            m_tool->hide();
            updateLayout(); // to update the minimum height
        }
    }
}

void KNote::updateLayout()
{
    // TODO: remove later if no longer needed.
    updateLabelAlignment();
}

// -------------------- protected methods -------------------- //

void KNote::contextMenuEvent(QContextMenuEvent *e)
{
    if (m_menu) {
        m_menu->popup(e->globalPos());
    }
}

void KNote::showEvent(QShowEvent *)
{
    if (mDisplayAttribute->isHidden()) {
        // KWin does not preserve these properties for hidden windows
        updateKeepAboveBelow(false);
#if KDEPIM_HAVE_X11
        slotUpdateShowInTaskbar();
#endif
        toDesktop(mDisplayAttribute->desktop());
        move(mDisplayAttribute->position());
        auto attr = mItem.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Item::AddIfMissing);
        saveNoteContent();
        attr->setIsHidden(false);
        if (!mBlockSave) {
            auto job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
            qCDebug(KNOTES_LOG) << "showEvent slotNoteSaved(KJob*)";
#endif
            connect(job, &Akonadi::ItemModifyJob::result, this, &KNote::slotNoteSaved);
        }
    }
}

void KNote::resizeEvent(QResizeEvent *qre)
{
    QFrame::resizeEvent(qre);
    updateLayout();
}

void KNote::closeEvent(QCloseEvent *event)
{
    if (qApp->isSavingSession()) {
        return;
    }
    event->ignore(); // We don't want to close (and delete the widget). Just hide it
    slotClose();
}

void KNote::dragEnterEvent(QDragEnterEvent *e)
{
    if (!m_editor->isReadOnly()) {
        e->setAccepted(e->mimeData()->hasColor());
    }
}

void KNote::dropEvent(QDropEvent *e)
{
    if (m_editor->isReadOnly()) {
        return;
    }

    const QMimeData *md = e->mimeData();
    if (md->hasColor()) {
        const auto bg = qvariant_cast<QColor>(md->colorData());

        auto attr = mItem.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Item::AddIfMissing);
        saveNoteContent();
        attr->setBackgroundColor(bg);
        auto job = new Akonadi::ItemModifyJob(mItem);
#ifdef DEBUG_SAVE_NOTE
        qCDebug(KNOTES_LOG) << "dropEvent slotNoteSaved(KJob*)";
#endif
        connect(job, &Akonadi::ItemModifyJob::result, this, &KNote::slotNoteSaved);
    }
}

bool KNote::event(QEvent *ev)
{
    if (ev->type() == QEvent::LayoutRequest) {
        updateLayout();
        return true;
    } else {
        return QFrame::event(ev);
    }
}

bool KNote::eventFilter(QObject *o, QEvent *ev)
{
    if (ev->type() == QEvent::DragEnter && static_cast<QDragEnterEvent *>(ev)->mimeData()->hasColor()) {
        dragEnterEvent(static_cast<QDragEnterEvent *>(ev));
        return true;
    }

    if (ev->type() == QEvent::Drop && static_cast<QDropEvent *>(ev)->mimeData()->hasColor()) {
        dropEvent(static_cast<QDropEvent *>(ev));
        return true;
    }

    if (o == m_label) {
        auto e = (QMouseEvent *)ev;

        if (ev->type() == QEvent::MouseButtonDblClick) {
            if (!m_editor->isReadOnly()) {
                slotRename();
            }
        }

        if (ev->type() == QEvent::MouseButtonPress
            && ((e->buttons() & Qt::LeftButton) == Qt::LeftButton || (e->buttons() & Qt::MiddleButton) == Qt::MiddleButton)) {
            mOrigPos = e->pos();
            return false;
        }

        if (ev->type() == QEvent::MouseMove && ((e->buttons() & Qt::LeftButton) == Qt::LeftButton || (e->buttons() & Qt::MiddleButton) == Qt::MiddleButton)) {
            QPoint newPos = e->globalPosition().toPoint() - mOrigPos - QPoint(1, 1);
            move(newPos);
            return true;
        }

        if (ev->type() == QEvent::MouseButtonRelease
            && ((e->buttons() & Qt::LeftButton) == Qt::LeftButton || (e->buttons() & Qt::MiddleButton) == Qt::MiddleButton)) {
            QPoint newPos = e->globalPosition().toPoint() - mOrigPos - QPoint(1, 1);
            move(newPos);
            return false;
        }
        return false;
    }

    if (o == m_editor) {
        if (ev->type() == QEvent::FocusOut) {
            auto fe = static_cast<QFocusEvent *>(ev);
            if (fe->reason() != Qt::PopupFocusReason && fe->reason() != Qt::MouseFocusReason) {
                updateFocus();
                if (!mBlockSave) {
                    saveNote(true);
                }
            }
        } else if (ev->type() == QEvent::FocusIn) {
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

void KNote::slotDebugAkonadiSearch()
{
    QPointer<Akonadi::Search::AkonadiSearchDebugDialog> dlg = new Akonadi::Search::AkonadiSearchDebugDialog;
    dlg->setAkonadiId(mItem.id());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setSearchType(Akonadi::Search::AkonadiSearchDebugSearchPathComboBox::Notes);
    dlg->doSearch();
    dlg->show();
}

#include "moc_knote.cpp"
