/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 2003, Daniel Martin <daniel.martin@pirack.com>
               2004, 2006, Michael Brade <brade@kde.org>
 Copyright (c) 2013, Laurent Montel <montel@kde.org>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

 In addition, as a special exception, the copyright holders give
 permission to link the code of this program with any edition of
 the Qt library by Trolltech AS, Norway (or with modified versions
 of Qt that use the same license as Qt), and distribute linked
 combinations including the two.  You must obey the GNU General
 Public License in all respects for all of the code used other than
 Qt.  If you modify this file, you may extend this exception to
 your version of the file, but you are not obligated to do so.  If
 you do not wish to do so, delete this exception statement from
 your version.
*******************************************************************/

#include "notesnetworksender.h"

#include <KLocalizedString>
#include <kmessagebox.h>

#include <QTextCodec>
using namespace NoteShared;
NotesNetworkSender::NotesNetworkSender(QTcpSocket *socket)
    : QObject(),
      m_socket(socket),
      m_note(),
      m_title(),
      m_sender()
{
    // QObject:: prefix needed, otherwise the KStreamSocket::connect()
    // method is called!!!
    QObject::connect(m_socket, &QTcpSocket::connected, this, &NotesNetworkSender::slotConnected);
    QObject::connect(m_socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &NotesNetworkSender::slotError);
    QObject::connect(m_socket, &QTcpSocket::disconnected, this, &NotesNetworkSender::slotClosed);
    QObject::connect(m_socket, &QTcpSocket::bytesWritten, this, &NotesNetworkSender::slotWritten);
}

NotesNetworkSender::~NotesNetworkSender()
{
    delete m_socket;
}

void NotesNetworkSender::setSenderId(const QString &sender)
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    m_sender = codec->fromUnicode(sender);
}

void NotesNetworkSender::setNote(const QString &title, const QString &text)
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    m_title = codec->fromUnicode(title);
    m_note = codec->fromUnicode(text);
}

void NotesNetworkSender::slotConnected()
{
    if (m_sender.isEmpty()) {
        m_note.prepend(m_title + "\n");
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
    KMessageBox::sorry(0, i18n("Communication error: %1",
                               m_socket->errorString()));
    slotClosed();
}

void NotesNetworkSender::slotClosed()
{
    deleteLater();
}

