/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "notesagentalarmdialog.h"
#include "alarms/notealarmdialog.h"
#include "attributes/notealarmattribute.h"
#include "notesagent_debug.h"
#include "notesagentnotedialog.h"
#include "widget/notelistwidget.h"
#include <KMime/KMimeMessage>

#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/ItemModifyJob>

#include <KLocalizedString>
#include <KMessageBox>
#include <QAction>
#include <QDateTime>
#include <QIcon>
#include <QMenu>

#include <KConfigGroup>
#include <KSharedConfig>
#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QLocale>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>

namespace
{
static const char myNotesAgentAlarmDialogName[] = "NotesAgentAlarmDialog";
}
NotesAgentAlarmDialog::NotesAgentAlarmDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Alarm"));
    setWindowIcon(QIcon::fromTheme(QStringLiteral("knotes")));
    setAttribute(Qt::WA_DeleteOnClose);
    auto mainLayout = new QVBoxLayout(this);

    auto vbox = new QVBoxLayout;
    mainLayout->addLayout(vbox);

    mCurrentDateTime = new QLabel(this);
    mCurrentDateTime->setText(QLocale().toString((QDateTime::currentDateTime()), QLocale::ShortFormat));
    vbox->addWidget(mCurrentDateTime);

    auto lab = new QLabel(i18n("The following notes triggered alarms:"), this);
    vbox->addWidget(lab);

    mListWidget = new NoteShared::NoteListWidget(this);
    mListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mListWidget, &NoteShared::NoteListWidget::itemDoubleClicked, this, &NotesAgentAlarmDialog::slotItemDoubleClicked);
    connect(mListWidget, &NoteShared::NoteListWidget::customContextMenuRequested, this, &NotesAgentAlarmDialog::slotCustomContextMenuRequested);

    mainLayout->addWidget(mListWidget);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &NotesAgentAlarmDialog::reject);
    buttonBox->button(QDialogButtonBox::Close)->setDefault(true);

    mainLayout->addWidget(buttonBox);
    readConfig();
}

NotesAgentAlarmDialog::~NotesAgentAlarmDialog()
{
    writeConfig();
}

void NotesAgentAlarmDialog::removeAlarm(const Akonadi::Item &note)
{
    mListWidget->removeNote(note);
    if (mListWidget->count() == 0) {
        close();
    }
}

void NotesAgentAlarmDialog::slotCustomContextMenuRequested(const QPoint &pos)
{
    if (mListWidget->selectedItems().isEmpty()) {
        return;
    }
    Q_UNUSED(pos)
    auto entriesContextMenu = new QMenu(this);
    auto removeAlarm = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove Alarm"), entriesContextMenu);
    connect(removeAlarm, &QAction::triggered, this, &NotesAgentAlarmDialog::slotRemoveAlarm);
    auto showNote = new QAction(i18nc("@action", "Show Note..."), entriesContextMenu);
    connect(showNote, &QAction::triggered, this, &NotesAgentAlarmDialog::slotShowNote);
    auto modifyAlarm = new QAction(i18nc("@action", "Modify Alarm..."), entriesContextMenu);
    connect(modifyAlarm, &QAction::triggered, this, &NotesAgentAlarmDialog::slotModifyAlarm);
    entriesContextMenu->addAction(showNote);
    entriesContextMenu->addAction(modifyAlarm);

    entriesContextMenu->addSeparator();
    entriesContextMenu->addAction(removeAlarm);
    entriesContextMenu->exec(QCursor::pos());
    delete entriesContextMenu;
}

void NotesAgentAlarmDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(300, 200));
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myNotesAgentAlarmDialogName));
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void NotesAgentAlarmDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myNotesAgentAlarmDialogName));
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}

void NotesAgentAlarmDialog::addListAlarm(const Akonadi::Item::List &lstAlarm)
{
    mListWidget->setNotes(lstAlarm);
    mCurrentDateTime->setText(QLocale().toString((QDateTime::currentDateTime()), QLocale::ShortFormat));
}

void NotesAgentAlarmDialog::slotItemDoubleClicked(QListWidgetItem *item)
{
    if (item) {
        slotShowNote();
    }
}

void NotesAgentAlarmDialog::slotShowNote()
{
    // deleted on close
    const Akonadi::Item::Id id = mListWidget->currentItemId();
    if (id != -1) {
        auto dlg = new NotesAgentNoteDialog;
        dlg->setNoteId(id);
        dlg->show();
    }
}

void NotesAgentAlarmDialog::slotRemoveAlarm()
{
    const int answer = KMessageBox::warningTwoActions(this,
                                                      i18n("Are you sure to remove alarm?"),
                                                      i18nc("@title:window", "Remove Alarm"),
                                                      KStandardGuiItem::remove(),
                                                      KStandardGuiItem::cancel());
    if (answer == KMessageBox::ButtonCode::PrimaryAction) {
        const Akonadi::Item::Id id = mListWidget->currentItemId();
        if (id != -1) {
            Akonadi::Item item(id);
            auto job = new Akonadi::ItemFetchJob(item, this);
            job->fetchScope().fetchAttribute<NoteShared::NoteAlarmAttribute>();
            connect(job, &Akonadi::ItemFetchJob::result, this, &NotesAgentAlarmDialog::slotFetchItem);
        }
    }
}

void NotesAgentAlarmDialog::slotFetchItem(KJob *job)
{
    if (job->error()) {
        qCDebug(NOTESAGENT_LOG) << "fetch item failed " << job->errorString();
        return;
    }
    auto itemFetchJob = static_cast<Akonadi::ItemFetchJob *>(job);
    Akonadi::Item::List items = itemFetchJob->items();
    if (!items.isEmpty()) {
        Akonadi::Item item = items.first();
        item.removeAttribute<NoteShared::NoteAlarmAttribute>();
        auto modify = new Akonadi::ItemModifyJob(item);
        connect(modify, &Akonadi::ItemModifyJob::result, this, &NotesAgentAlarmDialog::slotModifyItem);
    }
}

void NotesAgentAlarmDialog::slotModifyItem(KJob *job)
{
    if (job->error()) {
        qCDebug(NOTESAGENT_LOG) << "modify item failed " << job->errorString();
        return;
    }
}

void NotesAgentAlarmDialog::slotModifyAlarm()
{
    const Akonadi::Item::Id id = mListWidget->currentItemId();
    if (id != -1) {
        Akonadi::Item item(id);
        auto job = new Akonadi::ItemFetchJob(item, this);
        job->fetchScope().fetchFullPayload(true);
        job->fetchScope().fetchAttribute<NoteShared::NoteAlarmAttribute>();
        connect(job, &Akonadi::ItemFetchJob::result, this, &NotesAgentAlarmDialog::slotFetchAlarmItem);
    }
}

void NotesAgentAlarmDialog::slotFetchAlarmItem(KJob *job)
{
    if (job->error()) {
        qCDebug(NOTESAGENT_LOG) << "fetch item failed " << job->errorString();
        return;
    }
    auto itemFetchJob = static_cast<Akonadi::ItemFetchJob *>(job);
    Akonadi::Item::List items = itemFetchJob->items();
    if (!items.isEmpty()) {
        Akonadi::Item item = items.first();
        auto attr = item.attribute<NoteShared::NoteAlarmAttribute>();
        if (attr) {
            auto noteMessage = item.payload<KMime::Message::Ptr>();
            if (!noteMessage) {
                qCDebug(NOTESAGENT_LOG) << "Error this note doesn't have payload ";
                KMessageBox::error(this, i18n("Error during fetch alarm info."), i18n("Alarm"));
                return;
            }
            const KMime::Headers::Subject *const subject = noteMessage->subject(false);
            QString caption;
            if (subject) {
                caption = subject->asUnicodeString();
            }
            QPointer<NoteShared::NoteAlarmDialog> dlg = new NoteShared::NoteAlarmDialog(caption, this);
            dlg->setAlarm(attr->dateTime());
            if (dlg->exec()) {
                const QDateTime date = dlg->alarm();
                if (date.isValid()) {
                    attr->setDateTime(dlg->alarm());
                } else {
                    item.removeAttribute<NoteShared::NoteAlarmAttribute>();
                }
                auto modify = new Akonadi::ItemModifyJob(item);
                connect(modify, &Akonadi::ItemModifyJob::result, this, &NotesAgentAlarmDialog::slotModifyItem);
            }
            delete dlg;
        }
    }
}

#include "moc_notesagentalarmdialog.cpp"
