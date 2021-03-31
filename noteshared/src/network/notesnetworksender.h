/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 2003 Daniel Martin <daniel.martin@pirack.com>
 SPDX-FileCopyrightText: 2004, 2006 Michael Brade <brade@kde.org>
 SPDX-FileCopyrightText: 2013 Laurent Montel <montel@kde.org>

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#pragma once

#include <QTcpSocket>

namespace NoteShared
{
class NotesNetworkSender : public QObject
{
    Q_OBJECT
public:
    explicit NotesNetworkSender(QTcpSocket *socket);
    ~NotesNetworkSender() override;

    void setSenderId(const QString &sender);
    void setNote(const QString &title, const QString &text);

protected Q_SLOTS:
    void slotConnected();
    void slotError();
    void slotClosed();
    void slotWritten(qint64);

private:
    QTcpSocket *const m_socket;
    QByteArray m_note;
    QByteArray m_title;
    QByteArray m_sender;
};
}

