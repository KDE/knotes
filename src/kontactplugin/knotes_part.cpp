/*
  This file is part of the KDE project

  SPDX-FileCopyrightText: 2002-2003 Daniel Molkentin <molkentin@kde.org>
  SPDX-FileCopyrightText: 2004-2006 Michael Brade <brade@kde.org>
  SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knotes_part.h"
#include "akonadi/notesakonaditreemodel.h"
#include "akonadi/noteschangerecorder.h"
#include "alarms/notealarmdialog.h"
#include "attributes/notealarmattribute.h"
#include "attributes/notedisplayattribute.h"
#include "attributes/notelockattribute.h"
#include "attributes/showfoldernotesattribute.h"
#include "configdialog/knoteconfigdialog.h"
#include "configdialog/knotesimpleconfigdialog.h"
#include "finddialog/knotefinddialog.h"
#include "job/createnewnotejob.h"
#include "knoteedit.h"
#include "knotes_kontact_plugin_debug.h"
#include "knotesadaptor.h"
#include "knoteseditdialog.h"
#include "knotesglobalconfig.h"
#include "knotesiconview.h"
#include "knotesselectdeletenotesdialog.h"
#include "knoteswidget.h"
#include "notesharedglobalconfig.h"
#include "noteutils.h"
#include "print/knoteprinter.h"
#include "print/knoteprintobject.h"
#include "print/knoteprintselectthemedialog.h"
#include "resources/localresourcecreator.h"
#include "utils/knoteutils.h"

#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/ItemDeleteJob>
#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/ItemModifyJob>
#include <AkonadiCore/Session>
#include <AkonadiWidgets/ControlGui>
#include <AkonadiWidgets/ETMViewStateSaver>

#include <KMime/KMimeMessage>

#include <KActionCollection>
#include <KCheckableProxyModel>
#include <kdnssd_version.h>
#if KDNSSD_VERSION >= QT_VERSION_CHECK(5, 84, 0)
#include <KDNSSD/PublicService>
#else
#include <DNSSD/PublicService>
#endif
#include <KLocalizedString>
#include <KMessageBox>

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QFileDialog>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QMenu>

KNotesPart::KNotesPart(QObject *parent)
    : KParts::Part(parent)
    , mNotesWidget(nullptr)
    , mPublisher(nullptr)
    , mNotePrintPreview(nullptr)
    , mNoteTreeModel(nullptr)
{
    (void)new KNotesAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/KNotes"), this);

    setComponentName(QStringLiteral("knotes"), i18n("KNotes"));

    Akonadi::ControlGui::widgetNeedsAkonadi(widget());

    if (KNotesGlobalConfig::self()->autoCreateResourceOnStart()) {
        auto creator = new NoteShared::LocalResourceCreator(this);
        creator->createIfMissing();
    }

    // create the actions
    mNewNote = new QAction(QIcon::fromTheme(QStringLiteral("knotes")), i18nc("@action:inmenu create new popup note", "&New"), this);
    actionCollection()->addAction(QStringLiteral("file_new"), mNewNote);
    connect(mNewNote, &QAction::triggered, this, [this]() {
        newNote();
    });
    actionCollection()->setDefaultShortcut(mNewNote, QKeySequence(Qt::CTRL | Qt::Key_N));
    // mNewNote->setHelpText(
    //            i18nc( "@info:status", "Create a new popup note" ) );
    mNewNote->setWhatsThis(i18nc("@info:whatsthis", "You will be presented with a dialog where you can add a new popup note."));

    mNoteEdit = new QAction(QIcon::fromTheme(QStringLiteral("document-edit")), i18nc("@action:inmenu", "Edit..."), this);
    actionCollection()->addAction(QStringLiteral("edit_note"), mNoteEdit);
    connect(mNoteEdit, &QAction::triggered, this, [this]() {
        editNote();
    });
    // mNoteEdit->setHelpText(
    //            i18nc( "@info:status", "Edit popup note" ) );
    mNoteEdit->setWhatsThis(i18nc("@info:whatsthis", "You will be presented with a dialog where you can modify an existing popup note."));

    mNoteRename = new QAction(QIcon::fromTheme(QStringLiteral("edit-rename")), i18nc("@action:inmenu", "Rename..."), this);
    actionCollection()->setDefaultShortcut(mNoteRename, QKeySequence(Qt::Key_F2));
    actionCollection()->addAction(QStringLiteral("edit_rename"), mNoteRename);
    connect(mNoteRename, &QAction::triggered, this, &KNotesPart::renameNote);
    // mNoteRename->setHelpText(
    //            i18nc( "@info:status", "Rename popup note" ) );
    mNoteRename->setWhatsThis(i18nc("@info:whatsthis", "You will be presented with a dialog where you can rename an existing popup note."));

    mNoteDelete = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18nc("@action:inmenu", "Delete"), this);
    actionCollection()->addAction(QStringLiteral("edit_delete"), mNoteDelete);
    connect(mNoteDelete, &QAction::triggered, this, &KNotesPart::killSelectedNotes);
    actionCollection()->setDefaultShortcut(mNoteDelete, QKeySequence(Qt::Key_Delete));
    // mNoteDelete->setHelpText(
    //            i18nc( "@info:status", "Delete popup note" ) );
    mNoteDelete->setWhatsThis(i18nc("@info:whatsthis",
                                    "You will be prompted if you really want to permanently remove "
                                    "the selected popup note."));

    mNotePrint = new QAction(QIcon::fromTheme(QStringLiteral("document-print")), i18nc("@action:inmenu", "Print Selected Notes..."), this);
    actionCollection()->addAction(QStringLiteral("print_note"), mNotePrint);
    connect(mNotePrint, &QAction::triggered, this, &KNotesPart::slotPrintSelectedNotes);
    // mNotePrint->setHelpText(
    //            i18nc( "@info:status", "Print popup note" ) );
    mNotePrint->setWhatsThis(i18nc("@info:whatsthis", "You will be prompted to print the selected popup note."));

    mNotePrintPreview =
        new QAction(QIcon::fromTheme(QStringLiteral("document-print-preview")), i18nc("@action:inmenu", "Print Preview Selected Notes..."), this);
    actionCollection()->addAction(QStringLiteral("print_preview_note"), mNotePrintPreview);

    connect(mNotePrintPreview, &QAction::triggered, this, &KNotesPart::slotPrintPreviewSelectedNotes);

    mNoteConfigure = new QAction(QIcon::fromTheme(QStringLiteral("configure")), i18n("Note settings..."), this);
    actionCollection()->addAction(QStringLiteral("configure_note"), mNoteConfigure);
    connect(mNoteConfigure, &QAction::triggered, this, &KNotesPart::slotNotePreferences);

    auto act = new QAction(QIcon::fromTheme(QStringLiteral("configure")), i18n("Preferences KNotes..."), this);
    actionCollection()->addAction(QStringLiteral("knotes_configure"), act);
    connect(act, &QAction::triggered, this, &KNotesPart::slotPreferences);

    mNoteSendMail = new QAction(QIcon::fromTheme(QStringLiteral("mail-send")), i18n("Mail..."), this);
    actionCollection()->addAction(QStringLiteral("mail_note"), mNoteSendMail);
    connect(mNoteSendMail, &QAction::triggered, this, &KNotesPart::slotMail);

    mNoteSendNetwork = new QAction(QIcon::fromTheme(QStringLiteral("network-wired")), i18n("Send..."), this);
    actionCollection()->addAction(QStringLiteral("send_note"), mNoteSendNetwork);
    connect(mNoteSendNetwork, &QAction::triggered, this, &KNotesPart::slotSendToNetwork);

    mNoteSetAlarm = new QAction(QIcon::fromTheme(QStringLiteral("knotes_alarm")), i18n("Set Alarm..."), this);
    actionCollection()->addAction(QStringLiteral("set_alarm"), mNoteSetAlarm);
    connect(mNoteSetAlarm, &QAction::triggered, this, &KNotesPart::slotSetAlarm);

    act = new QAction(QIcon::fromTheme(QStringLiteral("edit-paste")), i18n("New Note From Clipboard"), this);
    actionCollection()->addAction(QStringLiteral("new_note_clipboard"), act);
    connect(act, &QAction::triggered, this, &KNotesPart::slotNewNoteFromClipboard);

    act = new QAction(QIcon::fromTheme(QStringLiteral("document-open")), i18n("New Note From Text File..."), this);
    actionCollection()->addAction(QStringLiteral("new_note_from_text_file"), act);
    connect(act, &QAction::triggered, this, &KNotesPart::slotNewNoteFromTextFile);

    mSaveAs = new QAction(QIcon::fromTheme(QStringLiteral("document-save-as")), i18n("Save As..."), this);
    actionCollection()->addAction(QStringLiteral("save_note"), mSaveAs);
    connect(mSaveAs, &QAction::triggered, this, &KNotesPart::slotSaveAs);

    mReadOnly = new KToggleAction(QIcon::fromTheme(QStringLiteral("object-locked")), i18n("Lock"), this);
    actionCollection()->addAction(QStringLiteral("lock_note"), mReadOnly);
    connect(mReadOnly, &KToggleAction::triggered, this, &KNotesPart::slotUpdateReadOnly);
    mReadOnly->setCheckedState(KGuiItem(i18n("Unlock"), QStringLiteral("object-unlocked")));

    KStandardAction::find(this, &KNotesPart::slotOpenFindDialog, actionCollection());

    auto session = new Akonadi::Session("KNotes Session", this);
    mNoteRecorder = new NoteShared::NotesChangeRecorder(this);
    mNoteRecorder->changeRecorder()->setSession(session);
    mNoteTreeModel = new NoteShared::NotesAkonadiTreeModel(mNoteRecorder->changeRecorder(), this);

    connect(mNoteTreeModel, &NoteShared::NotesAkonadiTreeModel::rowsInserted, this, &KNotesPart::slotRowInserted);

    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemChanged, this, &KNotesPart::slotItemChanged);

    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemRemoved, this, &KNotesPart::slotItemRemoved);

    connect(mNoteRecorder->changeRecorder(),
            qOverload<const Akonadi::Collection &, const QSet<QByteArray> &>(&Akonadi::ChangeRecorder::collectionChanged),
            this,
            &KNotesPart::slotCollectionChanged);

    mSelectionModel = new QItemSelectionModel(mNoteTreeModel);
    mModelProxy = new KCheckableProxyModel(this);
    mModelProxy->setSelectionModel(mSelectionModel);
    mModelProxy->setSourceModel(mNoteTreeModel);

    KSharedConfigPtr _config = KSharedConfig::openConfig(QStringLiteral("kcmknotessummaryrc"));

    mModelState = new KViewStateMaintainer<Akonadi::ETMViewStateSaver>(_config->group("CheckState"), this);
    mModelState->setSelectionModel(mSelectionModel);

    mNotesWidget = new KNotesWidget(this, widget());

    mQuickSearchAction = new QAction(i18n("Set Focus to Quick Search"), this);
    // If change shortcut change in quicksearchwidget->lineedit->setPlaceholderText
    actionCollection()->setDefaultShortcut(mQuickSearchAction, QKeySequence(Qt::ALT | Qt::Key_Q));
    actionCollection()->addAction(QStringLiteral("focus_to_quickseach"), mQuickSearchAction);
    connect(mQuickSearchAction, &QAction::triggered, mNotesWidget, &KNotesWidget::slotFocusQuickSearch);

    connect(mNotesWidget->notesView(), &QListWidget::itemDoubleClicked, this, qOverload<QListWidgetItem *>(&KNotesPart::editNote));

    connect(mNotesWidget->notesView(), &QListWidget::itemSelectionChanged, this, &KNotesPart::slotOnCurrentChanged);
    slotOnCurrentChanged();

    setWidget(mNotesWidget);
    setXMLFile(QStringLiteral("knotes_part.rc"));
    updateNetworkListener();
    updateClickMessage();
}

KNotesPart::~KNotesPart()
{
    delete mPublisher;
    mPublisher = nullptr;
}

void KNotesPart::updateClickMessage()
{
    mNotesWidget->updateClickMessage(mQuickSearchAction->shortcut().toString());
}

void KNotesPart::slotItemRemoved(const Akonadi::Item &item)
{
    KNotesIconViewItem *iconView = mNotesWidget->notesView()->iconView(item.id());
    delete iconView;
}

void KNotesPart::slotRowInserted(const QModelIndex &parent, int start, int end)
{
    for (int i = start; i <= end; ++i) {
        if (mNoteTreeModel->hasIndex(i, 0, parent)) {
            const QModelIndex child = mNoteTreeModel->index(i, 0, parent);
            auto parentCollection = mNoteTreeModel->data(child, Akonadi::EntityTreeModel::ParentCollectionRole).value<Akonadi::Collection>();
            if (parentCollection.hasAttribute<NoteShared::ShowFolderNotesAttribute>()) {
                auto item = mNoteTreeModel->data(child, Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
                if (!item.hasPayload<KMime::Message::Ptr>()) {
                    continue;
                }
                mNotesWidget->notesView()->addNote(item);
            }
        }
    }
}

QStringList KNotesPart::notesList() const
{
    QStringList notes;
    QHashIterator<Akonadi::Item::Id, KNotesIconViewItem *> i(mNotesWidget->notesView()->noteList());
    while (i.hasNext()) {
        i.next();
        notes.append(QString::number(i.key()));
    }
    return notes;
}

void KNotesPart::slotPrintPreviewSelectedNotes()
{
    printSelectedNotes(true);
}

void KNotesPart::slotPrintSelectedNotes()
{
    printSelectedNotes(false);
}

void KNotesPart::printSelectedNotes(bool preview)
{
    const QList<QListWidgetItem *> lst = mNotesWidget->notesView()->selectedItems();
    if (lst.isEmpty()) {
        KMessageBox::information(mNotesWidget,
                                 i18nc("@info", "To print notes, first select the notes to print from the list."),
                                 i18nc("@title:window", "Print Popup Notes"));
        return;
    }
    KNotesGlobalConfig *globalConfig = KNotesGlobalConfig::self();
    QString printingTheme = globalConfig->theme();
    if (printingTheme.isEmpty()) {
        QPointer<KNotePrintSelectThemeDialog> dlg = new KNotePrintSelectThemeDialog(widget());
        if (dlg->exec()) {
            printingTheme = dlg->selectedTheme();
        }
        delete dlg;
    }
    if (!printingTheme.isEmpty()) {
        QList<KNotePrintObject *> listPrintObj;
        listPrintObj.reserve(lst.count());
        for (QListWidgetItem *item : lst) {
            listPrintObj.append(new KNotePrintObject(static_cast<KNotesIconViewItem *>(item)->item()));
        }
        KNotePrinter printer;
        printer.printNotes(listPrintObj, printingTheme, preview);
        qDeleteAll(listPrintObj);
    }
}

// public KNotes D-Bus interface implementation

void KNotesPart::newNote(const QString &name, const QString &text)
{
    auto job = new NoteShared::CreateNewNoteJob(this, widget());
    job->setRichText(KNotesGlobalConfig::self()->richText());
    job->setNote(name, text);
    job->start();
}

void KNotesPart::slotNoteCreationFinished(KJob *job)
{
    if (job->error()) {
        qCWarning(KNOTES_KONTACT_PLUGIN_LOG) << job->errorString();
        NoteShared::NoteSharedGlobalConfig::self()->setDefaultFolder(-1);
        NoteShared::NoteSharedGlobalConfig::self()->save();
        KMessageBox::error(widget(), i18n("Note was not created."), i18n("Create new note"));
        return;
    }
}

void KNotesPart::newNoteFromClipboard(const QString &name)
{
    const QString &text = QApplication::clipboard()->text();
    newNote(name, text);
}

void KNotesPart::killNote(Akonadi::Item::Id id)
{
    killNote(id, false);
}

void KNotesPart::killNote(Akonadi::Item::Id id, bool force)
{
    KNotesIconViewItem *note = mNotesWidget->notesView()->iconView(id);
    if (note
        && ((!force
             && KMessageBox::warningContinueCancelList(mNotesWidget,
                                                       i18nc("@info", "Do you really want to delete this note?"),
                                                       QStringList(note->realName()),
                                                       i18nc("@title:window", "Confirm Delete"),
                                                       KStandardGuiItem::del())
                 == KMessageBox::Continue)
            || force)) {
        auto job = new Akonadi::ItemDeleteJob(note->item());
        connect(job, &Akonadi::ItemDeleteJob::result, this, &KNotesPart::slotDeleteNotesFinished);
    }
}

QString KNotesPart::name(Akonadi::Item::Id id) const
{
    KNotesIconViewItem *note = mNotesWidget->notesView()->iconView(id);
    if (note) {
        return note->text();
    } else {
        return QString();
    }
}

QString KNotesPart::text(Akonadi::Item::Id id) const
{
    // TODO return plaintext ?
    KNotesIconViewItem *note = mNotesWidget->notesView()->iconView(id);
    if (note) {
        return note->description();
    } else {
        return QString();
    }
}

void KNotesPart::setName(Akonadi::Item::Id id, const QString &newName)
{
    KNotesIconViewItem *note = mNotesWidget->notesView()->iconView(id);
    if (note) {
        note->setIconText(newName);
    }
}

void KNotesPart::setText(Akonadi::Item::Id id, const QString &newText)
{
    KNotesIconViewItem *note = mNotesWidget->notesView()->iconView(id);
    if (note) {
        note->setDescription(newText);
    }
}

QMap<QString, QString> KNotesPart::notes() const
{
    QMap<QString, QString> notes;
    QHashIterator<Akonadi::Item::Id, KNotesIconViewItem *> i(mNotesWidget->notesView()->noteList());
    while (i.hasNext()) {
        i.next();
        notes.insert(QString::number(i.key()), i.value()->realName());
    }
    return notes;
}

// private stuff

void KNotesPart::killSelectedNotes()
{
    const QList<QListWidgetItem *> lst = mNotesWidget->notesView()->selectedItems();
    if (lst.isEmpty()) {
        return;
    }
    QList<KNotesIconViewItem *> items;
    items.reserve(lst.count());
    for (QListWidgetItem *item : lst) {
        auto knivi = static_cast<KNotesIconViewItem *>(item);
        items.append(knivi);
    }

    if (items.isEmpty()) {
        return;
    }
    QPointer<KNotesSelectDeleteNotesDialog> dlg = new KNotesSelectDeleteNotesDialog(items, widget());
    if (dlg->exec()) {
        Akonadi::Item::List lst;
        QListIterator<KNotesIconViewItem *> kniviIt(items);
        while (kniviIt.hasNext()) {
            KNotesIconViewItem *iconViewIcon = kniviIt.next();
            if (!iconViewIcon->readOnly()) {
                lst.append(iconViewIcon->item());
            }
        }
        if (!lst.isEmpty()) {
            auto job = new Akonadi::ItemDeleteJob(lst);
            connect(job, &Akonadi::ItemDeleteJob::result, this, &KNotesPart::slotDeleteNotesFinished);
        }
    }
    delete dlg;
}

void KNotesPart::slotDeleteNotesFinished(KJob *job)
{
    if (job->error()) {
        qCDebug(KNOTES_KONTACT_PLUGIN_LOG) << " problem during delete job note:" << job->errorString();
    }
}

void KNotesPart::popupRMB(QListWidgetItem *item, const QPoint &pos, const QPoint &globalPos)
{
    Q_UNUSED(item)

    auto contextMenu = new QMenu(widget());
    if (mNotesWidget->notesView()->itemAt(pos)) {
        contextMenu->addAction(mNewNote);
        const bool uniqueNoteSelected = (mNotesWidget->notesView()->selectedItems().count() == 1);
        const bool readOnly = uniqueNoteSelected ? static_cast<KNotesIconViewItem *>(mNotesWidget->notesView()->selectedItems().at(0))->readOnly() : false;

        if (uniqueNoteSelected) {
            if (!readOnly) {
                contextMenu->addSeparator();
                contextMenu->addAction(mNoteSetAlarm);
            }
            contextMenu->addSeparator();
            contextMenu->addAction(mSaveAs);
            contextMenu->addSeparator();
            contextMenu->addAction(mNoteEdit);
            contextMenu->addAction(mReadOnly);
            if (!readOnly) {
                contextMenu->addAction(mNoteRename);
            }
            contextMenu->addSeparator();
            contextMenu->addAction(mNoteSendMail);
            contextMenu->addSeparator();
            contextMenu->addAction(mNoteSendNetwork);
        }
        contextMenu->addSeparator();
        contextMenu->addAction(mNotePrint);
        contextMenu->addAction(mNotePrintPreview);

        if (!readOnly) {
            contextMenu->addSeparator();
            contextMenu->addAction(mNoteConfigure);
            contextMenu->addSeparator();
            contextMenu->addAction(mNoteDelete);
        }
    } else {
        contextMenu->addAction(mNewNote);
    }

    contextMenu->exec(globalPos);
    delete contextMenu;
}

void KNotesPart::editNote(Akonadi::Item::Id id)
{
    KNotesIconViewItem *knoteItem = mNotesWidget->notesView()->iconView(id);
    if (knoteItem) {
        mNotesWidget->notesView()->setCurrentItem(knoteItem);
        editNote(knoteItem);
    }
}

void KNotesPart::editNote(QListWidgetItem *item)
{
    auto knotesItem = static_cast<KNotesIconViewItem *>(item);
    QPointer<KNoteEditDialog> dlg = new KNoteEditDialog(knotesItem->readOnly(), widget());
    dlg->setTitle(knotesItem->realName());
    dlg->setText(knotesItem->description());
    dlg->setColor(knotesItem->textForegroundColor(), knotesItem->textBackgroundColor());

    dlg->setAcceptRichText(knotesItem->isRichText());
    dlg->setTabSize(knotesItem->tabSize());
    dlg->setAutoIndentMode(knotesItem->autoIndent());
    dlg->setTextFont(knotesItem->textFont());

    dlg->setCursorPositionFromStart(knotesItem->cursorPositionFromStart());

    dlg->noteEdit()->setFocus();
    if (dlg->exec() == QDialog::Accepted) {
        knotesItem->setChangeIconTextAndDescription(dlg->title(), dlg->text(), dlg->cursorPositionFromStart());
    }
    delete dlg;
}

void KNotesPart::editNote()
{
    QListWidgetItem *item = mNotesWidget->notesView()->currentItem();
    if (item) {
        editNote(item);
    }
}

void KNotesPart::renameNote()
{
    auto knoteItem = static_cast<KNotesIconViewItem *>(mNotesWidget->notesView()->currentItem());

    const QString oldName = knoteItem->realName();
    bool ok = false;
    const QString newName =
        QInputDialog::getText(mNotesWidget, i18nc("@title:window", "Rename Popup Note"), i18nc("@label:textbox", "New Name:"), QLineEdit::Normal, oldName, &ok);
    if (ok && (newName != oldName)) {
        knoteItem->setIconText(newName);
    }
}

void KNotesPart::slotOnCurrentChanged()
{
    const bool uniqueNoteSelected = (mNotesWidget->notesView()->selectedItems().count() == 1);
    const bool enabled(mNotesWidget->notesView()->currentItem());
    mNoteRename->setEnabled(enabled && uniqueNoteSelected);
    mNoteEdit->setEnabled(enabled && uniqueNoteSelected);
    mNoteConfigure->setEnabled(uniqueNoteSelected);
    mNoteSendMail->setEnabled(uniqueNoteSelected);
    mNoteSendNetwork->setEnabled(uniqueNoteSelected);
    mNoteSetAlarm->setEnabled(uniqueNoteSelected);
    mSaveAs->setEnabled(uniqueNoteSelected);
    mReadOnly->setEnabled(uniqueNoteSelected);
    if (uniqueNoteSelected) {
        const bool readOnly = static_cast<KNotesIconViewItem *>(mNotesWidget->notesView()->selectedItems().at(0))->readOnly();
        mReadOnly->setChecked(readOnly);
        mNoteEdit->setText(readOnly ? i18n("Show Note...") : i18nc("@action:inmenu", "Edit..."));
    } else {
        mNoteEdit->setText(i18nc("@action:inmenu", "Edit..."));
    }
}

void KNotesPart::slotNotePreferences()
{
    if (!mNotesWidget->notesView()->currentItem()) {
        return;
    }
    auto knoteItem = static_cast<KNotesIconViewItem *>(mNotesWidget->notesView()->currentItem());
    QPointer<KNoteSimpleConfigDialog> dialog = new KNoteSimpleConfigDialog(knoteItem->realName(), widget());
    Akonadi::Item item = knoteItem->item();
    dialog->load(item, knoteItem->isRichText());
    if (dialog->exec()) {
        KNoteUtils::updateConfiguration();
        bool isRichText;
        dialog->save(item, isRichText);
        auto message = item.payload<KMime::Message::Ptr>();
        message->contentType(true)->setMimeType(isRichText ? "text/html" : "text/plain");
        message->assemble();
        auto job = new Akonadi::ItemModifyJob(item);
        connect(job, &Akonadi::ItemModifyJob::result, this, &KNotesPart::slotNoteSaved);
    }
    delete dialog;
}

void KNotesPart::slotPreferences()
{
    // create a new preferences dialog...
    auto dialog = new KNoteConfigDialog(i18n("Settings"), widget());
    connect(dialog, qOverload<>(&KCMultiDialog::configCommitted), this, &KNotesPart::slotConfigUpdated);
    dialog->show();
}

void KNotesPart::updateConfig()
{
    updateNetworkListener();
}

void KNotesPart::slotConfigUpdated()
{
    updateNetworkListener();
}

void KNotesPart::slotMail()
{
    if (!mNotesWidget->notesView()->currentItem()) {
        return;
    }
    auto knoteItem = static_cast<KNotesIconViewItem *>(mNotesWidget->notesView()->currentItem());
    NoteShared::NoteUtils noteUtils;
    noteUtils.sendToMail(widget(), knoteItem->realName(), knoteItem->description());
}

void KNotesPart::slotSendToNetwork()
{
    if (!mNotesWidget->notesView()->currentItem()) {
        return;
    }
    auto knoteItem = static_cast<KNotesIconViewItem *>(mNotesWidget->notesView()->currentItem());
    NoteShared::NoteUtils noteUtils;
    noteUtils.sendToNetwork(widget(), knoteItem->realName(), knoteItem->description());
}

void KNotesPart::updateNetworkListener()
{
    delete mPublisher;
    mPublisher = nullptr;

    if (NoteShared::NoteSharedGlobalConfig::receiveNotes()) {
        // create the socket and start listening for connections
        mPublisher = new KDNSSD::PublicService(NoteShared::NoteSharedGlobalConfig::senderID(),
                                               QStringLiteral("_knotes._tcp"),
                                               NoteShared::NoteSharedGlobalConfig::port());
        mPublisher->publishAsync();
    }
}

void KNotesPart::slotSetAlarm()
{
    if (!mNotesWidget->notesView()->currentItem()) {
        return;
    }
    auto knoteItem = static_cast<KNotesIconViewItem *>(mNotesWidget->notesView()->currentItem());
    QPointer<NoteShared::NoteAlarmDialog> dlg = new NoteShared::NoteAlarmDialog(knoteItem->realName(), widget());
    Akonadi::Item item = knoteItem->item();
    if (item.hasAttribute<NoteShared::NoteAlarmAttribute>()) {
        dlg->setAlarm(item.attribute<NoteShared::NoteAlarmAttribute>()->dateTime());
    }
    if (dlg->exec()) {
        bool needToModify = true;
        QDateTime dateTime = dlg->alarm();
        if (dateTime.isValid()) {
            auto attribute = item.attribute<NoteShared::NoteAlarmAttribute>(Akonadi::Item::AddIfMissing);
            attribute->setDateTime(dateTime);
        } else {
            if (item.hasAttribute<NoteShared::NoteAlarmAttribute>()) {
                item.removeAttribute<NoteShared::NoteAlarmAttribute>();
            } else {
                needToModify = false;
            }
        }
        if (needToModify) {
            auto job = new Akonadi::ItemModifyJob(item);
            connect(job, &Akonadi::ItemModifyJob::result, this, &KNotesPart::slotNoteSaved);
        }
    }
    delete dlg;
}

void KNotesPart::slotNoteSaved(KJob *job)
{
    qCDebug(KNOTES_KONTACT_PLUGIN_LOG) << " void KNote::slotNoteSaved(KJob *job)";
    if (job->error()) {
        qCDebug(KNOTES_KONTACT_PLUGIN_LOG) << " problem during save note:" << job->errorString();
    }
}

void KNotesPart::slotNewNoteFromClipboard()
{
    const QString &text = QApplication::clipboard()->text();
    newNote(QString(), text);
}

void KNotesPart::slotSaveAs()
{
    if (!mNotesWidget->notesView()->currentItem()) {
        return;
    }
    auto knoteItem = static_cast<KNotesIconViewItem *>(mNotesWidget->notesView()->currentItem());

    QStringList filters;
    if (knoteItem->isRichText()) {
        filters << i18n("Rich text (*.html)");
    }
    filters << i18n("Plain text (*.txt)");

    QString format;
    const QString fileName = QFileDialog::getSaveFileName(widget(), i18n("Save As"), QString(), filters.join(QLatin1String(";;")), &format);
    if (fileName.isEmpty()) {
        return;
    }
    const bool htmlFormatAndSaveAsHtml = (knoteItem->isRichText() && !format.contains(QLatin1String("(*.txt)")));

    QFile file(fileName);
    if (file.exists()
        && KMessageBox::warningContinueCancel(widget(),
                                              i18n("<qt>A file named <b>%1</b> already exists.<br />"
                                                   "Are you sure you want to overwrite it?</qt>",
                                                   QFileInfo(file).fileName()))
            != KMessageBox::Continue) {
        return;
    }

    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        QTextDocument doc;
        doc.setHtml(knoteItem->description());
        if (htmlFormatAndSaveAsHtml) {
            QString htmlStr = doc.toHtml();
            htmlStr.replace(QStringLiteral("meta name=\"qrichtext\" content=\"1\""),
                            QStringLiteral("meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\""));
            stream << htmlStr;
        } else {
            stream << knoteItem->realName() + QLatin1Char('\n');
            stream << doc.toPlainText();
        }
    }
}

void KNotesPart::slotUpdateReadOnly()
{
    QListWidgetItem *item = mNotesWidget->notesView()->currentItem();
    if (!item) {
        return;
    }
    auto knoteItem = static_cast<KNotesIconViewItem *>(item);

    const bool readOnly = mReadOnly->isChecked();

    mNoteEdit->setText(readOnly ? i18n("Show Note...") : i18nc("@action:inmenu", "Edit..."));
    knoteItem->setReadOnly(readOnly);
}

void KNotesPart::slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &set)
{
    KNotesIconViewItem *knoteItem = mNotesWidget->notesView()->iconView(item.id());
    if (knoteItem) {
        knoteItem->setChangeItem(item, set);
    }
}

void KNotesPart::slotOpenFindDialog()
{
    if (!mNoteFindDialog) {
        mNoteFindDialog = new KNoteFindDialog(widget());
        connect(mNoteFindDialog.data(), &KNoteFindDialog::noteSelected, this, &KNotesPart::slotSelectNote);
    }
    QHash<Akonadi::Item::Id, Akonadi::Item> lst;
    QHashIterator<Akonadi::Item::Id, KNotesIconViewItem *> i(mNotesWidget->notesView()->noteList());
    while (i.hasNext()) {
        i.next();
        lst.insert(i.key(), i.value()->item());
    }
    mNoteFindDialog->setExistingNotes(lst);
    mNoteFindDialog->show();
}

void KNotesPart::slotSelectNote(Akonadi::Item::Id id)
{
    editNote(id);
}

void KNotesPart::slotCollectionChanged(const Akonadi::Collection &col, const QSet<QByteArray> &set)
{
    if (set.contains("showfoldernotesattribute")) {
        // qCDebug(KNOTES_KONTACT_PLUGIN_LOG)<<" collection Changed "<<set<<" col "<<col;
        if (col.hasAttribute<NoteShared::ShowFolderNotesAttribute>()) {
            fetchNotesFromCollection(col);
        } else {
            QHashIterator<Akonadi::Item::Id, KNotesIconViewItem *> i(mNotesWidget->notesView()->noteList());
            while (i.hasNext()) {
                i.next();
                Akonadi::Item item = i.value()->item();
                if (item.parentCollection() == col) {
                    slotItemRemoved(item);
                }
            }
        }
    }
}

void KNotesPart::fetchNotesFromCollection(const Akonadi::Collection &col)
{
    auto job = new Akonadi::ItemFetchJob(col);
    job->fetchScope().fetchFullPayload(true);
    job->fetchScope().fetchAttribute<NoteShared::NoteLockAttribute>();
    job->fetchScope().fetchAttribute<NoteShared::NoteDisplayAttribute>();
    job->fetchScope().fetchAttribute<NoteShared::NoteAlarmAttribute>();
    job->fetchScope().setAncestorRetrieval(Akonadi::ItemFetchScope::Parent);
    connect(job, &Akonadi::ItemFetchJob::result, this, &KNotesPart::slotItemFetchFinished);
}

void KNotesPart::slotItemFetchFinished(KJob *job)
{
    if (job->error()) {
        qCDebug(KNOTES_KONTACT_PLUGIN_LOG) << "Error occurred during item fetch:" << job->errorString();
        return;
    }

    auto fetchJob = qobject_cast<Akonadi::ItemFetchJob *>(job);

    const Akonadi::Item::List items = fetchJob->items();
    for (const Akonadi::Item &item : items) {
        if (!item.hasPayload<KMime::Message::Ptr>()) {
            continue;
        }
        mNotesWidget->notesView()->addNote(item);
    }
}

void KNotesPart::slotNewNoteFromTextFile()
{
    QString text;
    const QString filename = QFileDialog::getOpenFileName(widget(), i18n("Select Text File"), QString(), QStringLiteral("%1 (*.txt)").arg(i18n("Text File")));
    if (!filename.isEmpty()) {
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            text = QString::fromUtf8(f.readAll());
        } else {
            KMessageBox::error(widget(), i18n("Error during open text file: %1", f.errorString()), i18n("Open Text File"));
            return;
        }
        newNote(i18n("Note from file '%1'", filename), text);
    }
}
