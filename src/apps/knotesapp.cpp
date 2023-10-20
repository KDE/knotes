/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2013 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include <config-knotes.h>

#include "akonadi/notesakonaditreemodel.h"
#include "akonadi/noteschangerecorder.h"
#include "apps/knotesakonaditray.h"
#include "attributes/notealarmattribute.h"
#include "attributes/notedisplayattribute.h"
#include "attributes/notelockattribute.h"
#include "attributes/showfoldernotesattribute.h"
#include "configdialog/knoteconfigdialog.h"
#include "dialog/knotedeleteselectednotesdialog.h"
#include "dialog/knoteskeydialog.h"
#include "finddialog/knotefinddialog.h"
#include "job/createnewnotejob.h"
#include "knotes_debug.h"
#include "knotesadaptor.h"
#include "knotesapp.h"
#include "knotesglobalconfig.h"
#include "notes/knote.h"
#include "notesharedglobalconfig.h"
#include "print/knoteprinter.h"
#include "print/knoteprintselectednotesdialog.h"
#include "resources/localresourcecreator.h"
#include "utils/knoteutils.h"

#include <Akonadi/ChangeRecorder>
#include <Akonadi/ItemDeleteJob>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/Session>

#include <Akonadi/ControlGui>

#include <KMime/KMimeMessage>

#include <KActionCollection>
#include <KDNSSD/PublicService>
#include <KGlobalAccel>
#include <KIconEffect>
#include <KLocalizedString>
#include <KMessageBox>
#include <KWindowSystem>
#include <KXMLGUIBuilder>
#include <KXMLGUIFactory>

#include <KAboutData>
#include <KHelpMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDBusConnection>
#include <QFileDialog>
#include <QMenu>
#include <QStyle>
#if KDEPIM_HAVE_X11
#include <KWindowInfo>
#include <KX11Extras>
#endif

static bool qActionLessThan(const QAction *a1, const QAction *a2)
{
    return a1->text() < a2->text();
}

KNotesApp::KNotesApp(QWidget *parent)
    : QWidget(parent)
{
    Akonadi::ControlGui::widgetNeedsAkonadi(this);

    mDebugAkonadiSearch = !qEnvironmentVariableIsEmpty("KDEPIM_DEBUGGING");

    if (KNotesGlobalConfig::self()->autoCreateResourceOnStart()) {
        auto creator = new NoteShared::LocalResourceCreator(this);
        creator->createIfMissing();
    }

    new KNotesAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/KNotes"), this);
    qApp->setQuitOnLastWindowClosed(false);
    // create the GUI...
    auto action = new QAction(QIcon::fromTheme(QStringLiteral("document-new")), i18n("New Note"), this);
    actionCollection()->addAction(QStringLiteral("new_note"), action);
    KGlobalAccel::setGlobalShortcut(action, QKeySequence(Qt::ALT | Qt::SHIFT | Qt::Key_N));
    connect(action, &QAction::triggered, this, [this]() {
        newNote();
    });

    action = new QAction(QIcon::fromTheme(QStringLiteral("edit-paste")), i18n("New Note From Clipboard"), this);
    actionCollection()->addAction(QStringLiteral("new_note_clipboard"), action);
    KGlobalAccel::setGlobalShortcut(action, QKeySequence(Qt::ALT | Qt::SHIFT | Qt::Key_C));
    connect(action, &QAction::triggered, this, &KNotesApp::newNoteFromClipboard);

    action = new QAction(QIcon::fromTheme(QStringLiteral("document-open")), i18n("New Note From Text File..."), this);
    actionCollection()->addAction(QStringLiteral("new_note_from_text_file"), action);
    connect(action, &QAction::triggered, this, &KNotesApp::newNoteFromTextFile);

    action = new QAction(QIcon::fromTheme(QStringLiteral("knotes")), i18n("Show All Notes"), this);
    actionCollection()->addAction(QStringLiteral("show_all_notes"), action);
    KGlobalAccel::setGlobalShortcut(action, QKeySequence(Qt::ALT | Qt::SHIFT | Qt::Key_S));
    connect(action, &QAction::triggered, this, &KNotesApp::showAllNotes);

    action = new QAction(QIcon::fromTheme(QStringLiteral("window-close")), i18n("Hide All Notes"), this);
    actionCollection()->addAction(QStringLiteral("hide_all_notes"), action);
    KGlobalAccel::setGlobalShortcut(action, QKeySequence(Qt::ALT | Qt::SHIFT | Qt::Key_H));
    connect(action, &QAction::triggered, this, &KNotesApp::hideAllNotes);

    action = new QAction(QIcon::fromTheme(QStringLiteral("document-print")), i18nc("@action:inmenu", "Print Selected Notes..."), this);
    actionCollection()->addAction(QStringLiteral("print_selected_notes"), action);
    connect(action, &QAction::triggered, this, &KNotesApp::slotPrintSelectedNotes);

    QAction *act = KStandardAction::find(this, &KNotesApp::slotOpenFindDialog, actionCollection());
    action = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18nc("@action:inmenu", "Delete Selected Notes..."), this);
    actionCollection()->addAction(QStringLiteral("delete_selected_notes"), action);
    connect(action, &QAction::triggered, this, &KNotesApp::slotDeleteSelectedNotes);

    // REmove shortcut here.
    act->setShortcut(0);

    auto menu = new KHelpMenu(this, KAboutData::applicationData(), false);

    KActionCollection *actions = actionCollection();
    QAction *helpContentsAction = menu->action(KHelpMenu::menuHelpContents);
    QAction *whatsThisAction = menu->action(KHelpMenu::menuWhatsThis);
    QAction *reportBugAction = menu->action(KHelpMenu::menuReportBug);
    QAction *switchLanguageAction = menu->action(KHelpMenu::menuSwitchLanguage);
    QAction *aboutAppAction = menu->action(KHelpMenu::menuAboutApp);
    QAction *aboutKdeAction = menu->action(KHelpMenu::menuAboutKDE);
    QAction *donateAction = menu->action(KHelpMenu::menuDonate);

    if (helpContentsAction) {
        actions->addAction(helpContentsAction->objectName(), helpContentsAction);
    }
    if (whatsThisAction) {
        actions->addAction(whatsThisAction->objectName(), whatsThisAction);
    }
    if (reportBugAction) {
        actions->addAction(reportBugAction->objectName(), reportBugAction);
    }
    if (switchLanguageAction) {
        actions->addAction(switchLanguageAction->objectName(), switchLanguageAction);
    }
    if (aboutAppAction) {
        actions->addAction(aboutAppAction->objectName(), aboutAppAction);
    }
    if (aboutKdeAction) {
        actions->addAction(aboutKdeAction->objectName(), aboutKdeAction);
    }
    if (donateAction) {
        actions->addAction(donateAction->objectName(), donateAction);
    }

    KStandardAction::preferences(this, &KNotesApp::slotPreferences, actionCollection());
    KStandardAction::keyBindings(this, &KNotesApp::slotConfigureAccels, actionCollection());
    // FIXME: no shortcut removing!?
    KStandardAction::quit(this, &KNotesApp::slotQuit, actionCollection())->setShortcut(0);
    setXMLFile(QStringLiteral("knotesappui.rc"));

    m_guiBuilder = new KXMLGUIBuilder(this);
    m_guiFactory = new KXMLGUIFactory(m_guiBuilder, this);
    m_guiFactory->addClient(this);

    QMenu *contextMenu = static_cast<QMenu *>(m_guiFactory->container(QStringLiteral("knotes_context"), this));
    m_noteMenu = static_cast<QMenu *>(m_guiFactory->container(QStringLiteral("notes_menu"), this));

    // get the most recent XML UI file
    QString xmlFileName(componentName() + QLatin1String("ui.rc"));
#pragma message("port QT6")
    QString filter(QStringLiteral("kxmlgui5/knotes/") + xmlFileName); // QT5 = componentData().componentName() + QLatin1Char('/') + xmlFileName;
    const QStringList fileList = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, filter)
        + QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, xmlFileName); // QT5 =
#pragma message("port QT6")
    // QT5 componentData().dirs()->findAllResources( "data", filter ) +
#pragma message("port QT6")
    // QT5 componentData().dirs()->findAllResources( "data", xmlFileName );
    qCDebug(KNOTES_LOG) << " fileList :" << fileList << " filter :" << filter;
    QString doc;
    KXMLGUIClient::findMostRecentXMLFile(fileList, doc);
    m_noteGUI.setContent(doc);
    // set up the alarm reminder - do it after loading the notes because this
    // is used as a check if updateNoteActions has to be called for a new note
    updateNetworkListener();

    auto session = new Akonadi::Session("KNotes Session", this);
    mNoteRecorder = new NoteShared::NotesChangeRecorder(this);
    mNoteRecorder->changeRecorder()->setSession(session);
    mTray = new KNotesAkonadiTray(nullptr);

    connect(mTray, &KStatusNotifierItem::activateRequested, this, &KNotesApp::slotActivateRequested);

    connect(mTray, &KStatusNotifierItem::secondaryActivateRequested, this, &KNotesApp::slotSecondaryActivateRequested);

    mTray->setContextMenu(contextMenu);
    mNoteTreeModel = new NoteShared::NotesAkonadiTreeModel(mNoteRecorder->changeRecorder(), this);

    connect(mNoteTreeModel, &QAbstractItemModel::rowsInserted, this, &KNotesApp::slotRowInserted);

    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemChanged, this, &KNotesApp::slotItemChanged);

    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemRemoved, this, &KNotesApp::slotItemRemoved);

    connect(mNoteRecorder->changeRecorder(),
            qOverload<const Akonadi::Collection &, const QSet<QByteArray> &>(&Akonadi::ChangeRecorder::collectionChanged),
            this,
            &KNotesApp::slotCollectionChanged);

    connect(qApp, &QGuiApplication::commitDataRequest, this, &KNotesApp::slotCommitData, Qt::DirectConnection);

    updateNoteActions();
}

KNotesApp::~KNotesApp()
{
    qDeleteAll(m_noteActions);
    m_noteActions.clear();
    saveNotes();
    delete m_guiBuilder;
    delete mTray;
    qDeleteAll(mNotes);
    mNotes.clear();
    delete m_publisher;
    m_publisher = nullptr;
}

void KNotesApp::slotGeneralPaletteChanged()
{
    mTray->slotGeneralPaletteChanged();
    mTray->updateNumberOfNotes(mNotes.count());
}

bool KNotesApp::event(QEvent *e)
{
    if (e->type() == QEvent::ApplicationPaletteChange) {
        slotGeneralPaletteChanged();
    }
    return QWidget::event(e);
}

void KNotesApp::slotDeleteSelectedNotes()
{
    QPointer<KNoteDeleteSelectedNotesDialog> dlg = new KNoteDeleteSelectedNotesDialog(this);
    Akonadi::Item::List lst;
    QHashIterator<Akonadi::Item::Id, KNote *> i(mNotes);
    while (i.hasNext()) {
        i.next();
        Akonadi::Item item = i.value()->item();
        if (!item.hasAttribute<NoteShared::NoteLockAttribute>()) {
            lst.append(item);
        }
    }
    dlg->setNotes(lst);
    if (dlg->exec()) {
        const Akonadi::Item::List lstItem = dlg->selectedNotes();
        if (!lstItem.isEmpty()) {
            auto deleteJob = new Akonadi::ItemDeleteJob(lstItem, this);
            connect(deleteJob, &KJob::result, this, &KNotesApp::slotNoteDeleteFinished);
        }
    }
    delete dlg;
}

void KNotesApp::slotItemRemoved(const Akonadi::Item &item)
{
    qCDebug(KNOTES_LOG) << " note removed" << item.id();
    if (mNotes.contains(item.id())) {
        delete mNotes.find(item.id()).value();
        mNotes.remove(item.id());
        updateNoteActions();
        updateSystray();
    }
}

void KNotesApp::slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &set)
{
    if (mNotes.contains(item.id())) {
        qCDebug(KNOTES_LOG) << " item changed " << item.id() << " info " << set.values();
        KNote *note = mNotes.value(item.id());
        note->setChangeItem(item, set);
    }
}

void KNotesApp::slotRowInserted(const QModelIndex &parent, int start, int end)
{
    bool needUpdate = false;
    for (int i = start; i <= end; ++i) {
        if (mNoteTreeModel->hasIndex(i, 0, parent)) {
            const QModelIndex child = mNoteTreeModel->index(i, 0, parent);
            auto item = mNoteTreeModel->data(child, Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
            auto parentCollection = mNoteTreeModel->data(child, Akonadi::EntityTreeModel::ParentCollectionRole).value<Akonadi::Collection>();
            if (parentCollection.hasAttribute<NoteShared::ShowFolderNotesAttribute>()) {
                createNote(item);
                needUpdate = true;
            }
        }
    }
    if (needUpdate) {
        updateNoteActions();
        updateSystray();
    }
}

void KNotesApp::createNote(const Akonadi::Item &item)
{
    if (item.hasPayload<KMime::Message::Ptr>() && !mNotes.contains(item.id())) {
        auto note = new KNote(m_noteGUI, item, mDebugAkonadiSearch);
        mNotes.insert(item.id(), note);
        connect(note, &KNote::sigShowNextNote, this, &KNotesApp::slotWalkThroughNotes);
        connect(note, &KNote::sigRequestNewNote, this, [this] {
            newNote();
        });
        connect(note, &KNote::sigNameChanged, this, &KNotesApp::updateNoteActions);
        connect(note, &KNote::sigColorChanged, this, &KNotesApp::updateNoteActions);
        connect(note, &KNote::sigKillNote, this, &KNotesApp::slotNoteKilled);
    }
}

void KNotesApp::updateSystray()
{
    if (KNotesGlobalConfig::self()->systemTrayShowNotes()) {
        mTray->updateNumberOfNotes(mNotes.count());
    }
}

void KNotesApp::newNote(const QString &name, const QString &text)
{
    auto job = new NoteShared::CreateNewNoteJob(this, this);
    job->setRichText(KNotesGlobalConfig::self()->richText());
    job->setNote(name, text);
    job->start();
}

void KNotesApp::showNote(Akonadi::Item::Id id) const
{
    KNote *note = mNotes.value(id);
    if (note) {
        showNote(note);
    } else {
        qCWarning(KNOTES_LOG) << "hideNote: no note with id:" << id;
    }
}

void KNotesApp::showNote(KNote *note) const
{
    note->show();
#if KDEPIM_HAVE_X11
    if (!note->isDesktopAssigned()) {
        note->toDesktop(KX11Extras::currentDesktop());
    } else {
        KX11Extras::setCurrentDesktop(KWindowInfo(note->winId(), NET::WMDesktop).desktop());
    }
    KX11Extras::forceActiveWindow(note->winId());
#endif
    note->setFocus();
}

void KNotesApp::hideNote(Akonadi::Item::Id id) const
{
    KNote *note = mNotes.value(id);
    if (note) {
        note->hide();
    } else {
        qCWarning(KNOTES_LOG) << "hideNote: no note with id:" << id;
    }
}

void KNotesApp::hideAllNotes() const
{
    QHashIterator<Akonadi::Item::Id, KNote *> i(mNotes);
    while (i.hasNext()) {
        i.next();
        i.value()->slotClose();
    }
}

void KNotesApp::showAllNotes() const
{
    QHashIterator<Akonadi::Item::Id, KNote *> i(mNotes);
    while (i.hasNext()) {
        i.next();
        // workaround to BUG 149116
        i.value()->hide();

        i.value()->show();
    }
}

void KNotesApp::newNoteFromClipboard()
{
    const QString &text = QApplication::clipboard()->text();
    newNote(QString(), text);
}

void KNotesApp::newNoteFromTextFile()
{
    QString text;
    const QString filename = QFileDialog::getOpenFileName(this, i18n("Select Text File"), QString(), QStringLiteral("%1 (*.txt)").arg(i18n("Text File")));
    if (!filename.isEmpty()) {
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            text = QString::fromUtf8(f.readAll());
        } else {
            KMessageBox::error(this, i18n("Error during open text file: %1", f.errorString()), i18n("Open Text File"));
            return;
        }
        newNote(i18n("Note from file '%1'", filename), text);
    }
}

void KNotesApp::updateNetworkListener()
{
    delete m_publisher;
    m_publisher = nullptr;

    if (NoteShared::NoteSharedGlobalConfig::receiveNotes()) {
        // create the socket and start listening for connections
        m_publisher = new KDNSSD::PublicService(NoteShared::NoteSharedGlobalConfig::senderID(),
                                                QStringLiteral("_knotes._tcp"),
                                                NoteShared::NoteSharedGlobalConfig::port());
        m_publisher->publishAsync();
    }
}

QString KNotesApp::name(Akonadi::Item::Id id) const
{
    KNote *note = mNotes.value(id);
    if (note) {
        return note->name();
    }
    return {};
}

QString KNotesApp::text(Akonadi::Item::Id id) const
{
    KNote *note = mNotes.value(id);
    if (note) {
        return note->text();
    }
    return {};
}

void KNotesApp::setName(Akonadi::Item::Id id, const QString &newName)
{
    KNote *note = mNotes.value(id);
    if (note) {
        note->setName(newName);
    } else {
        qCWarning(KNOTES_LOG) << "setName: no note with id:" << id;
    }
}

void KNotesApp::setText(Akonadi::Item::Id id, const QString &newText)
{
    KNote *note = mNotes.value(id);
    if (note) {
        note->setText(newText);
    } else {
        qCWarning(KNOTES_LOG) << "setText: no note with id:" << id;
    }
}

void KNotesApp::updateNoteActions()
{
    unplugActionList(QStringLiteral("notes"));
    m_noteActions.clear();

    QHashIterator<Akonadi::Item::Id, KNote *> i(mNotes);
    while (i.hasNext()) {
        i.next();
        KNote *note = i.value();
        QString replaceText;
        QString realName = note->name();
        if (realName.length() > 50) {
            replaceText = realName.left(50) + QLatin1String("...");
        } else {
            replaceText = realName;
        }

        auto action = new QAction(replaceText.replace(QLatin1String("&"), QStringLiteral("&&")), this);
        action->setToolTip(realName);
        action->setObjectName(QString::number(note->noteId()));
        connect(action, &QAction::triggered, this, &KNotesApp::slotShowNote);
        KIconEffect effect;
        QPixmap icon = effect.apply(qApp->windowIcon().pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize)),
                                    KIconEffect::Colorize,
                                    1,
                                    note->palette().color(note->backgroundRole()),
                                    false);

        action->setIcon(icon);
        m_noteActions.append(action);
    }

    if (m_noteActions.isEmpty()) {
        actionCollection()->action(QStringLiteral("hide_all_notes"))->setEnabled(false);
        actionCollection()->action(QStringLiteral("show_all_notes"))->setEnabled(false);
        actionCollection()->action(QStringLiteral("print_selected_notes"))->setEnabled(false);
        actionCollection()->action(QStringLiteral("edit_find"))->setEnabled(false);
        auto action = new QAction(i18n("No Notes"), this);
        action->setEnabled(false);
        m_noteActions.append(action);
    } else {
        std::sort(m_noteActions.begin(), m_noteActions.end(), qActionLessThan);
        actionCollection()->action(QStringLiteral("hide_all_notes"))->setEnabled(true);
        actionCollection()->action(QStringLiteral("show_all_notes"))->setEnabled(true);
        actionCollection()->action(QStringLiteral("print_selected_notes"))->setEnabled(true);
        actionCollection()->action(QStringLiteral("edit_find"))->setEnabled(true);
    }
    plugActionList(QStringLiteral("notes"), m_noteActions);
}

void KNotesApp::slotActivateRequested(bool, const QPoint &)
{
    if (mNotes.size() == 1) {
        showNote(mNotes.begin().value());
    } else {
        m_noteMenu->popup(QCursor::pos());
    }
}

void KNotesApp::slotSecondaryActivateRequested(const QPoint &)
{
    newNote();
}

void KNotesApp::slotShowNote()
{
    // tell the WM to give this note focus
    showNote(sender()->objectName().toLongLong());
}

void KNotesApp::slotWalkThroughNotes()
{
    QHashIterator<Akonadi::Item::Id, KNote *> i(mNotes);
    while (i.hasNext()) {
        i.next();
        KNote *note = i.value();
        if (note->hasFocus()) {
            if (i.value() != mNotes.end().value()) {
                showNote(i.value());
            } else {
                showNote(mNotes.begin().value());
            }
            break;
        }
    }
}

void KNotesApp::slotPreferences()
{
    // create a new preferences dialog...
    auto dialog = new KNoteConfigDialog(i18n("Settings"), this);
    connect(dialog, qOverload<>(&KCMultiDialog::configCommitted), this, &KNotesApp::slotConfigUpdated);
    dialog->show();
}

void KNotesApp::slotConfigUpdated()
{
    updateNetworkListener();
    KNoteUtils::updateConfiguration();
    // Force update if we disable or enable show number in systray
    mTray->updateNumberOfNotes(mNotes.count());
}

void KNotesApp::slotCollectionChanged(const Akonadi::Collection &col, const QSet<QByteArray> &set)
{
    if (set.contains("showfoldernotesattribute")) {
        // qCDebug(KNOTES_LOG)<<" collection Changed "<<set<<" col "<<col;
        if (col.hasAttribute<NoteShared::ShowFolderNotesAttribute>()) {
            fetchNotesFromCollection(col);
        } else {
            QHashIterator<Akonadi::Item::Id, KNote *> i(mNotes);
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

void KNotesApp::slotConfigureAccels()
{
    QPointer<KNotesKeyDialog> keys = new KNotesKeyDialog(actionCollection(), this);

    KActionCollection *actionCollection = nullptr;
    if (!mNotes.isEmpty()) {
        actionCollection = mNotes.begin().value()->actionCollection();
        keys->insert(actionCollection);
    }
    if (keys->exec()) {
        keys->save();
        // update GUI doc for new notes
        m_noteGUI.setContent(KXMLGUIFactory::readConfigFile(componentName() + QLatin1String("ui.rc"), componentName()));

        if (actionCollection) {
            QHashIterator<Akonadi::Item::Id, KNote *> i(mNotes);
            while (i.hasNext()) {
                i.next();
                const auto lst = actionCollection->actions();
                for (QAction *action : lst) {
                    QAction *toChange = i.value()->actionCollection()->action(action->objectName());
                    if (toChange) {
                        toChange->setShortcuts(action->shortcuts());
                    }
                }
            }
        }
    }
    delete keys;
}

void KNotesApp::slotNoteKilled(Akonadi::Item::Id id)
{
    auto deleteJob = new Akonadi::ItemDeleteJob(Akonadi::Item(id), this);
    connect(deleteJob, &KJob::result, this, &KNotesApp::slotNoteDeleteFinished);
}

void KNotesApp::slotNoteDeleteFinished(KJob *job)
{
    if (job->error()) {
        qCWarning(KNOTES_LOG) << job->errorString();
        return;
    }
}

void KNotesApp::slotPrintSelectedNotes()
{
    QPointer<KNotePrintSelectedNotesDialog> dlg = new KNotePrintSelectedNotesDialog(this);
    dlg->setNotes(mNotes);
    if (dlg->exec()) {
        const QList<KNotePrintObject *> lst = dlg->selectedNotes();
        if (!lst.isEmpty()) {
            const QString selectedTheme = dlg->selectedTheme();
            KNotePrinter printer;
            printer.printNotes(lst, selectedTheme, dlg->preview());
            qDeleteAll(lst);
        }
    }
    delete dlg;
}

void KNotesApp::saveNotes(bool force, bool sync)
{
    KNotesGlobalConfig::self()->save();
    QHashIterator<Akonadi::Item::Id, KNote *> i(mNotes);
    while (i.hasNext()) {
        i.next();
        i.value()->saveNote(force, sync);
    }
}

void KNotesApp::slotQuit()
{
    saveNotes(true, true);
    qApp->quit();
}

void KNotesApp::slotCommitData(QSessionManager &)
{
    saveNotes(true, true);
}

void KNotesApp::slotSelectNote(Akonadi::Item::Id id)
{
    showNote(id);
}

void KNotesApp::slotOpenFindDialog()
{
    if (!mFindDialog) {
        mFindDialog = new KNoteFindDialog(this);
        connect(mFindDialog.data(), &KNoteFindDialog::noteSelected, this, &KNotesApp::slotSelectNote);
    }
    QHash<Akonadi::Item::Id, Akonadi::Item> lst;

    QHashIterator<Akonadi::Item::Id, KNote *> i(mNotes);
    while (i.hasNext()) {
        i.next();
        lst.insert(i.key(), i.value()->item());
    }
    mFindDialog->setExistingNotes(lst);
    mFindDialog->show();
}

void KNotesApp::fetchNotesFromCollection(const Akonadi::Collection &col)
{
    auto job = new Akonadi::ItemFetchJob(col);
    job->fetchScope().fetchFullPayload(true);
    job->fetchScope().fetchAttribute<NoteShared::NoteLockAttribute>();
    job->fetchScope().fetchAttribute<NoteShared::NoteDisplayAttribute>();
    job->fetchScope().fetchAttribute<NoteShared::NoteAlarmAttribute>();
    job->fetchScope().setAncestorRetrieval(Akonadi::ItemFetchScope::Parent);
    connect(job, &KJob::result, this, &KNotesApp::slotItemFetchFinished);
}

void KNotesApp::slotItemFetchFinished(KJob *job)
{
    if (job->error()) {
        qCDebug(KNOTES_LOG) << "Error occurred during item fetch:" << job->errorString();
        return;
    }

    auto fetchJob = qobject_cast<Akonadi::ItemFetchJob *>(job);

    const Akonadi::Item::List items = fetchJob->items();
    for (const Akonadi::Item &item : items) {
        createNote(item);
    }
    if (!items.isEmpty()) {
        updateNoteActions();
        updateSystray();
    }
}

#include "moc_knotesapp.cpp"
