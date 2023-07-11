/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 2003 Daniel Martin <daniel.martin@pirack.com>
 SPDX-FileCopyrightText: 2004, 2006 Michael Brade <brade@kde.org>
 SPDX-FileCopyrightText: 2013 Laurent Montel <montel@kde.org>

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include "notesnetworksender.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QStringEncoder>

using namespace NoteShared;

NotesNetworkSender::NotesNetworkSender(QTcpSocket *socket)
    : QObject()
    , m_socket(socket)
{
    // QObject:: prefix needed, otherwise the KStreamSocket::connect()
    // method is called!!!
    QObject::connect(m_socket, &QTcpSocket::connected, this, &NotesNetworkSender::slotConnected);

    QObject::connect(m_socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::errorOccurred), this, &NotesNetworkSender::slotError);

    QObject::connect(m_socket, &QTcpSocket::disconnected, this, &NotesNetworkSender::slotClosed);

    QObject::connect(m_socket, &QTcpSocket::bytesWritten, this, &NotesNetworkSender::slotWritten);
}

NotesNetworkSender::~NotesNetworkSender()
{
    delete m_socket;
}

void NotesNetworkSender::setSenderId(const QString &sender)
{
    QStringEncoder codec(QStringEncoder::System);
    m_sender = codec.encode(sender);
}

void NotesNetworkSender::setNote(const QString &title, const QString &text)
{
    QStringEncoder codec(QStringEncoder::System);
    m_title = codec.encode(title);
    m_note = codec.encode(text);
}

void NotesNetworkSender::slotConnected()
{
    if (m_sender.isEmpty()) {
        m_note.prepend(m_title + '\n');
    } else {
        m_note.prepend(m_title + " (" + m_sender + ")\n");
    }

    m_socket->write(m_note);
}

void NotesNetworkSender::slotWritten(qint64)
{
    // If end of text reached, close connection
    if (m_socket->bytesToWrite() == 0) {
        m_socket->close();
    }
}

void NotesNetworkSender::slotError()
{
    KMessageBox::error(nullptr, i18n("Communication error: %1", m_socket->errorString()));
    slotClosed();
}

void NotesNetworkSender::slotClosed()
{
    deleteLater();
}

#include "moc_notesnetworksender.cpp"
