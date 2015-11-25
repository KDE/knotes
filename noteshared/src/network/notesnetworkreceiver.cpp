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

#include "notesnetworkreceiver.h"

#include <QDateTime>
#include <QHostAddress>
#include <QRegExp>
#include <QTcpSocket>
#include <QTimer>
#include <QTextCodec>
#include "noteshared_debug.h"
#include <KLocalizedString>
#include <QLocale>

// Maximum note size in chars we are going to accept,
// to prevent "note floods".
#define MAXBUFFER 4096

// Maximum time we are going to wait between data receptions,
// to prevent memory and connection floods. In milliseconds.
#define MAXTIME 10000

// Small buffer's size
#define SBSIZE 512
using namespace NoteShared;

class NoteShared::NotesNetworkReceiverPrivate
{
public:
    NotesNetworkReceiverPrivate(QTcpSocket *s)
        : m_timer(Q_NULLPTR),
          m_buffer(new QByteArray()),
          m_sock(s)
    {

    }
    ~NotesNetworkReceiverPrivate()
    {
        delete m_buffer;
        delete m_sock;
    }

    QTimer *m_timer;       // to avoid memory and connection floods

    QByteArray *m_buffer;
    QTcpSocket *m_sock;

    QString m_titleAddon;
};

NotesNetworkReceiver::NotesNetworkReceiver(QTcpSocket *s)
    : QObject(),
      d(new NoteShared::NotesNetworkReceiverPrivate(s))
{
    const QString date = QLocale().toString(QDateTime::currentDateTime(), QLocale::ShortFormat);

    // Add the remote IP or hostname and the date to the title, to help the
    // user guess who wrote it.
    d->m_titleAddon = QStringLiteral(" [%1, %2]")
                      .arg(d->m_sock->peerAddress().toString())
                      .arg(date);

    // Setup the communications
    connect(d->m_sock, &QTcpSocket::readyRead, this, &NotesNetworkReceiver::slotDataAvailable);
    connect(d->m_sock, &QTcpSocket::disconnected, this, &NotesNetworkReceiver::slotConnectionClosed);
    connect(d->m_sock, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &NotesNetworkReceiver::slotError);

    // Setup the timer
    d->m_timer = new QTimer(this);
    d->m_timer->setSingleShot(true);
    connect(d->m_timer, &QTimer::timeout, this, &NotesNetworkReceiver::slotReceptionTimeout);
    d->m_timer->start(MAXTIME);
}

NotesNetworkReceiver::~NotesNetworkReceiver()
{
    delete d;
}

void NotesNetworkReceiver::slotDataAvailable()
{
    char smallBuffer[SBSIZE];
    int smallBufferLen;

    do {
        // Append to "big buffer" only if we have some space left.
        int curLen = d->m_buffer->count();

        smallBufferLen = d->m_sock->read(smallBuffer, SBSIZE);

        // Limit max transfer over buffer, to avoid overflow.
        smallBufferLen = qMin(smallBufferLen, MAXBUFFER - curLen);

        if (smallBufferLen > 0) {
            d->m_buffer->resize(curLen + smallBufferLen);
            memcpy(d->m_buffer->data() + curLen, smallBuffer, smallBufferLen);
        }
    } while (smallBufferLen == SBSIZE);

    // If we are overflowing, close connection.
    if (d->m_buffer->count() == MAXBUFFER) {
        d->m_sock->close();
    } else {
        d->m_timer->start(MAXTIME);
    }
}

void NotesNetworkReceiver::slotReceptionTimeout()
{
    d->m_sock->close();
}

void NotesNetworkReceiver::slotConnectionClosed()
{
    QTextCodec *codec = QTextCodec::codecForLocale();

    if (d->m_timer->isActive()) {
        QString noteText = QString(codec->toUnicode(*d->m_buffer)).trimmed();

        // First line is the note title or, in case of ATnotes, the id
        const int pos = noteText.indexOf(QRegExp(QLatin1String("[\r\n]")));
        const QString noteTitle = noteText.left(pos).trimmed() + d->m_titleAddon;

        noteText = noteText.mid(pos).trimmed();

        if (!noteText.isEmpty()) {
            Q_EMIT sigNoteReceived(noteTitle, noteText);
        }
    }

    deleteLater();
}

void NotesNetworkReceiver::slotError(QAbstractSocket::SocketError error)
{
    qCWarning(NOTESHARED_LOG) << "error type :" << (int) error << " error string : " << d->m_sock->errorString();
}

