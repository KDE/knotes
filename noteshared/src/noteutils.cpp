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
#include "noteutils.h"
#include "network/notesnetworksender.h"
#include "network/notehostdialog.h"
#include "attributes/notedisplayattribute.h"
#include "notesharedglobalconfig.h"
#include <KProcess>
#include <KMessageBox>
#include <KLocalizedString>

#include <KMime/KMimeMessage>

#include <QPointer>
#include <QApplication>
#include <QSslSocket>

using namespace NoteShared;

NoteUtils::NoteUtils()
{

}

bool NoteUtils::sendToMail(QWidget *parent, const QString &title, const QString &message)
{
    // get the mail action command
    const QStringList cmd_list = NoteShared::NoteSharedGlobalConfig::mailAction().split(QLatin1Char(' '), QString::SkipEmptyParts);
    if (cmd_list.isEmpty()) {
        KMessageBox::sorry(parent, i18n("Please configure send mail action."));
        return false;
    }
    KProcess mail;
    Q_FOREACH (const QString &cmd, cmd_list) {
        if (cmd == QLatin1String("%f")) {
            mail << message;
        } else if (cmd == QLatin1String("%t")) {
            mail << i18n("Note: \"%1\"", title);
        } else {
            mail << cmd;
        }
    }

    if (!mail.startDetached()) {
        KMessageBox::sorry(parent, i18n("Unable to start the mail process."));
        return false;
    }
    return true;
}

void NoteUtils::sendToNetwork(QWidget *parent, const QString &title, const QString &message)
{
    // pop up dialog to get the IP
    QPointer<NoteShared::NoteHostDialog> hostDlg = new NoteShared::NoteHostDialog(i18n("Send \"%1\"", title), parent);
    if (hostDlg->exec()) {

        const QString host = hostDlg->host();
        if (host.isEmpty()) {
            KMessageBox::sorry(parent, i18n("The host cannot be empty."));
            delete hostDlg;
            return;
        }
        quint16 port = hostDlg->port();

        if (!port) {   // not specified, use default
            port = NoteShared::NoteSharedGlobalConfig::port();
        }

        // Send the note
        auto socket = new QSslSocket;
        socket->connectToHost(host, port);
        NoteShared::NotesNetworkSender *sender = new NoteShared::NotesNetworkSender(socket);
        sender->setSenderId(NoteShared::NoteSharedGlobalConfig::senderID());
        sender->setNote(title, message);   // FIXME: plainText ??
    }
    delete hostDlg;
}

QString NoteUtils::createToolTip(const Akonadi::Item &item)
{
    const KMime::Message::Ptr noteMessage = item.payload<KMime::Message::Ptr>();
    if (!noteMessage) {
        return QString();
    }
    const QString description = QString::fromLatin1(noteMessage->mainBodyPart()->decodedContent());
    const KMime::Headers::Subject *const subject = noteMessage->subject(false);

    const QString realName = subject ? subject->asUnicodeString() : QString();
    const bool isRichText = noteMessage->contentType()->isHTMLText();

    QString tip;
    if (item.hasAttribute<NoteDisplayAttribute>()) {
        NoteDisplayAttribute *attr = item.attribute<NoteDisplayAttribute>();
        if (attr) {
            const QString bckColorName = attr->backgroundColor().name();
            const QString txtColorName = attr->foregroundColor().name();;
            const bool textIsLeftToRight = (QApplication::layoutDirection() == Qt::LeftToRight);
            const QString textDirection =  textIsLeftToRight ? QStringLiteral("left") : QStringLiteral("right");

            tip = QStringLiteral(
                      "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">"
                  );
            tip += QStringLiteral(
                       "<tr>" \
                       "<td bgcolor=\"%1\" align=\"%4\" valign=\"middle\">" \
                       "<div style=\"color: %2; font-weight: bold;\">" \
                       "%3" \
                       "</div>" \
                       "</td>" \
                       "</tr>"
                   ).arg(bckColorName).arg(txtColorName).arg(realName.toHtmlEscaped()).arg(textDirection);
            const QString htmlCodeForStandardRow = QStringLiteral(
                    "<tr>" \
                    "<td bgcolor=\"%1\" align=\"left\" valign=\"top\">" \
                    "<div style=\"color: %2;\">" \
                    "%3" \
                    "</div>" \
                    "</td>" \
                    "</tr>");

            QString content = description;
            if (!content.trimmed().isEmpty()) {
                tip += htmlCodeForStandardRow.arg(bckColorName).arg(txtColorName).arg(isRichText ? content : content.replace(QLatin1Char('\n'), QStringLiteral("<br>")));
            }
            tip += QLatin1String(
                       "</table" \
                       "</td>" \
                       "</tr>"
                   );
        }
    }
    return tip;
}

