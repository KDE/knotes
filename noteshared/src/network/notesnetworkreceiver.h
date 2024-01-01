/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 2003 Daniel Martin <daniel.martin@pirack.com>
 SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel.org>

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#pragma once

#include "noteshared_export.h"
#include <QAbstractSocket>
#include <QObject>

#include <memory>

class QTcpSocket;
namespace NoteShared
{
class NotesNetworkReceiverPrivate;
class NOTESHARED_EXPORT NotesNetworkReceiver : public QObject
{
    Q_OBJECT
public:
    explicit NotesNetworkReceiver(QTcpSocket *);
    ~NotesNetworkReceiver() override;

Q_SIGNALS:
    void sigNoteReceived(const QString &, const QString &);

private Q_SLOTS:
    void slotDataAvailable();
    void slotReceptionTimeout();
    void slotConnectionClosed();
    void slotError(QAbstractSocket::SocketError);

private:
    std::unique_ptr<NotesNetworkReceiverPrivate> const d;
};
}
