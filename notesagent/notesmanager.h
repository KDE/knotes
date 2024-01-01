/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QObject>

#include "notesagentalarmdialog.h"

#include <Akonadi/Item>
#include <QPointer>
class QTcpServer;
class QTimer;
namespace NoteShared
{
class NotesChangeRecorder;
}

namespace Akonadi
{
class Session;
}

class NotesManager : public QObject
{
    Q_OBJECT
public:
    explicit NotesManager(QObject *parent = nullptr);
    ~NotesManager() override;

    void stopAll();
    void updateNetworkListener();

public Q_SLOTS:
    void load();

private:
    void slotAcceptConnection();
    void slotNewNote(const QString &name, const QString &text);
    void slotCheckAlarm();

    void slotItemAdded(const Akonadi::Item &item);
    void slotItemRemoved(const Akonadi::Item &item);
    void slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &set);

    void slotCollectionsReceived(const Akonadi::Collection::List &collections);

    void clear();
    Akonadi::Item::List mListItem;
    QTcpServer *mListener = nullptr;
    QTimer *mCheckAlarm = nullptr;
    NoteShared::NotesChangeRecorder *mNoteRecorder = nullptr;
    Akonadi::Session *const mSession;
    QPointer<NotesAgentAlarmDialog> mAlarmDialog;
};
