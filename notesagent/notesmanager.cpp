/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "notesmanager.h"
#include "akonadi/noteschangerecorder.h"
#include "attributes/notealarmattribute.h"
#include "attributes/notedisplayattribute.h"
#include "attributes/notelockattribute.h"
#include "job/createnewnotejob.h"
#include "network/notesnetworkreceiver.h"
#include "notesharedglobalconfig.h"

#include <Akonadi/Notes/NoteUtils>
#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/Collection>
#include <AkonadiCore/CollectionFetchJob>
#include <AkonadiCore/CollectionFetchScope>
#include <AkonadiCore/Item>
#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/Session>

#include <KMime/KMimeMessage>

#include <KLocalizedString>
#include <KNotification>

#include <QDateTime>
#include <QTcpServer>
#include <QTimer>

NotesManager::NotesManager(QObject *parent)
    : QObject(parent)
{
    mSession = new Akonadi::Session("KNotes Session", this);
    mNoteRecorder = new NoteShared::NotesChangeRecorder(this);
    mNoteRecorder->changeRecorder()->setSession(mSession);
    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemAdded, this, &NotesManager::slotItemAdded);
    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemChanged, this, &NotesManager::slotItemChanged);
    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemRemoved, this, &NotesManager::slotItemRemoved);

    auto job = new Akonadi::CollectionFetchJob(Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, mSession);
    job->fetchScope().setContentMimeTypes({Akonadi::NoteUtils::noteMimeType()});
    job->fetchScope().setFetchIdOnly(true);
    connect(job, &Akonadi::CollectionFetchJob::collectionsReceived, this, &NotesManager::slotCollectionsReceived);
}

NotesManager::~NotesManager()
{
    clear();
}

void NotesManager::clear()
{
    delete mListener;
    mListener = nullptr;
    if (mCheckAlarm && mCheckAlarm->isActive()) {
        mCheckAlarm->stop();
    }
}

void NotesManager::slotCollectionsReceived(const Akonadi::Collection::List &collections)
{
    for (const Akonadi::Collection &col : collections) {
        if (!col.contentMimeTypes().contains(Akonadi::NoteUtils::noteMimeType())) {
            continue;
        }
        auto job = new Akonadi::ItemFetchJob(col, mSession);
        job->setDeliveryOption(Akonadi::ItemFetchJob::EmitItemsInBatches);
        job->fetchScope().fetchAttribute<NoteShared::NoteAlarmAttribute>();
        job->fetchScope().fetchAttribute<NoteShared::NoteLockAttribute>();
        job->fetchScope().fetchAttribute<NoteShared::NoteDisplayAttribute>();
        job->fetchScope().fetchAttribute<NoteShared::NoteAlarmAttribute>();
        job->fetchScope().fetchFullPayload(true);
        connect(job, &Akonadi::ItemFetchJob::itemsReceived, this, [this](const Akonadi::Item::List &items) {
            for (const Akonadi::Item &item : items) {
                slotItemAdded(item);
            }
        });
    }
}

void NotesManager::slotItemAdded(const Akonadi::Item &item)
{
    if (item.hasAttribute<NoteShared::NoteAlarmAttribute>()) {
        mListItem.append(item);
    }
}

void NotesManager::slotItemRemoved(const Akonadi::Item &item)
{
    mListItem.removeAll(item);
}

void NotesManager::slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &set)
{
    if (set.contains("ATR:NoteAlarmAttribute")) {
        mListItem.removeAll(item);
        if (item.hasAttribute<NoteShared::NoteAlarmAttribute>()) {
            mListItem.append(item);
        } else {
            if (mAlarmDialog) {
                mAlarmDialog->removeAlarm(item);
            }
        }
    }
}

void NotesManager::slotCheckAlarm()
{
    QDateTime from = NoteShared::NoteSharedGlobalConfig::self()->alarmsLastChecked().addSecs(1);
    if (!from.isValid()) {
        from.setSecsSinceEpoch(0);
    }

    const QDateTime now = QDateTime::currentDateTime();
    NoteShared::NoteSharedGlobalConfig::self()->setAlarmsLastChecked(now);

    Akonadi::Item::List lst;
    for (const Akonadi::Item &item : std::as_const(mListItem)) {
        const auto attrAlarm = item.attribute<NoteShared::NoteAlarmAttribute>();
        if (attrAlarm) {
            if (attrAlarm->dateTime() < QDateTime::currentDateTime()) {
                lst.append(item);
            }
        }
    }
    if (!lst.isEmpty()) {
        if (!mAlarmDialog) {
            mAlarmDialog = new NotesAgentAlarmDialog;
        }
        mAlarmDialog->addListAlarm(lst);
        mAlarmDialog->show();
    }
    mCheckAlarm->start();
}

void NotesManager::load()
{
    updateNetworkListener();
    if (!mCheckAlarm) {
        mCheckAlarm = new QTimer(this);
    } else if (mCheckAlarm->isActive()) {
        mCheckAlarm->stop();
    }

    mCheckAlarm->setInterval(1000 * 60 * NoteShared::NoteSharedGlobalConfig::checkInterval());
    connect(mCheckAlarm, &QTimer::timeout, this, &NotesManager::slotCheckAlarm);
    slotCheckAlarm();
}

void NotesManager::stopAll()
{
    clear();
}

void NotesManager::slotAcceptConnection()
{
    // Accept the connection and make KNotesNetworkReceiver do the job
    QTcpSocket *s = mListener->nextPendingConnection();

    if (s) {
        auto recv = new NoteShared::NotesNetworkReceiver(s);
        connect(recv, &NoteShared::NotesNetworkReceiver::sigNoteReceived, this, &NotesManager::slotNewNote);
    }
}

void NotesManager::slotNewNote(const QString &name, const QString &text)
{
    KNotification::event(QStringLiteral("receivednotes"),
                         QString(),
                         i18n("Note Received"),
                         QStringLiteral("knotes"),
                         nullptr,
                         KNotification::CloseOnTimeout,
                         QStringLiteral("akonadi_notes_agent"));
    auto job = new NoteShared::CreateNewNoteJob(this, nullptr);
    // For the moment it doesn't support richtext.
    job->setRichText(false);
    job->setNote(name, text);
    job->start();
}

void NotesManager::updateNetworkListener()
{
    delete mListener;
    mListener = nullptr;

    if (NoteShared::NoteSharedGlobalConfig::receiveNotes()) {
        // create the socket and start listening for connections
        mListener = new QTcpServer;
        mListener->listen(QHostAddress::Any, NoteShared::NoteSharedGlobalConfig::port());
        connect(mListener, &QTcpServer::newConnection, this, &NotesManager::slotAcceptConnection);
    }
}
