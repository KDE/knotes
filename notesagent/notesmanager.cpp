/*
   Copyright (C) 2013-2016 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "notesmanager.h"
#include "notesharedglobalconfig.h"
#include "network/notesnetworkreceiver.h"
#include "job/createnewnotejob.h"
#include "akonadi/noteschangerecorder.h"
#include "akonadi/notesakonaditreemodel.h"
#include "attributes/notealarmattribute.h"
#include "notesagentalarmdialog.h"

#include <AkonadiCore/Session>
#include <AkonadiCore/Collection>
#include <AkonadiCore/Item>
#include <AkonadiCore/ChangeRecorder>

#include <KMime/KMimeMessage>

#include <KNotification>
#include <KIconLoader>
#include <KLocalizedString>

#include <QDateTime>
#include <QIcon>
#include <QTcpServer>
#include <QTimer>

NotesManager::NotesManager(QObject *parent)
    : QObject(parent),
      mListener(Q_NULLPTR),
      mCheckAlarm(Q_NULLPTR)
{
    Akonadi::Session *session = new Akonadi::Session("KNotes Session", this);
    mNoteRecorder = new NoteShared::NotesChangeRecorder(this);
    mNoteRecorder->changeRecorder()->setSession(session);
    mNoteTreeModel = new NoteShared::NotesAkonadiTreeModel(mNoteRecorder->changeRecorder(), this);

    connect(mNoteTreeModel, &NoteShared::NotesAkonadiTreeModel::rowsInserted, this, &NotesManager::slotRowInserted);

    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemChanged, this, &NotesManager::slotItemChanged);
    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemRemoved, this, &NotesManager::slotItemRemoved);
}

NotesManager::~NotesManager()
{
    clear();
}

void NotesManager::clear()
{
    delete mListener;
    mListener = Q_NULLPTR;
    if (mCheckAlarm && mCheckAlarm->isActive()) {
        mCheckAlarm->stop();
    }
}

void NotesManager::slotItemRemoved(const Akonadi::Item &item)
{
    if (mListItem.contains(item)) {
        mListItem.removeAll(item);
    }
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

void NotesManager::slotRowInserted(const QModelIndex &parent, int start, int end)
{
    for (int i = start; i <= end; ++i) {
        if (mNoteTreeModel->hasIndex(i, 0, parent)) {
            const QModelIndex child = mNoteTreeModel->index(i, 0, parent);
            Akonadi::Item item =
                mNoteTreeModel->data(child, Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
            if (!item.hasPayload<KMime::Message::Ptr>()) {
                continue;
            }
            if (item.hasAttribute<NoteShared::NoteAlarmAttribute>()) {
                mListItem.append(item);
            }
        }
    }
}

void NotesManager::slotCheckAlarm()
{
    QDateTime from = NoteShared::NoteSharedGlobalConfig::self()->alarmsLastChecked().addSecs(1);
    if (!from.isValid()) {
        from.setTime_t(0);
    }

    const QDateTime now = QDateTime::currentDateTime();
    NoteShared::NoteSharedGlobalConfig::self()->setAlarmsLastChecked(now);

    Akonadi::Item::List lst;
    Q_FOREACH (const Akonadi::Item &item, mListItem) {
        NoteShared::NoteAlarmAttribute *attrAlarm = item.attribute<NoteShared::NoteAlarmAttribute>();
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
        NoteShared::NotesNetworkReceiver *recv = new NoteShared::NotesNetworkReceiver(s);
        connect(recv, &NoteShared::NotesNetworkReceiver::sigNoteReceived, this, &NotesManager::slotNewNote);
    }
}

void NotesManager::slotNewNote(const QString &name, const QString &text)
{
    const QPixmap pixmap = QIcon::fromTheme(QStringLiteral("knotes")).pixmap(KIconLoader::SizeSmall, KIconLoader::SizeSmall);
    KNotification::event(QStringLiteral("receivednotes"),
                         i18n("Note Received"),
                         pixmap,
                         Q_NULLPTR,
                         KNotification::CloseOnTimeout,
                         QStringLiteral("akonadi_notes_agent"));
    NoteShared::CreateNewNoteJob *job = new NoteShared::CreateNewNoteJob(this, Q_NULLPTR);
    //For the moment it doesn't support richtext.
    job->setRichText(false);
    job->setNote(name, text);
    job->start();
}

void NotesManager::updateNetworkListener()
{
    delete mListener;
    mListener = Q_NULLPTR;

    if (NoteShared::NoteSharedGlobalConfig::receiveNotes()) {
        // create the socket and start listening for connections
        mListener = new QTcpServer;
        mListener->listen(QHostAddress::Any, NoteShared::NoteSharedGlobalConfig::port());
        connect(mListener, &QTcpServer::newConnection, this, &NotesManager::slotAcceptConnection);
    }
}
