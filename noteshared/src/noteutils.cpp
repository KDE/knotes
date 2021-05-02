/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "noteutils.h"
#include "attributes/notedisplayattribute.h"
#include "network/notehostdialog.h"
#include "network/notesnetworksender.h"
#include "notesharedglobalconfig.h"
#include <KLocalizedString>
#include <KMessageBox>
#include <KProcess>

#include <KMime/KMimeMessage>

#include <QApplication>
#include <QPointer>
#include <QRegularExpression>
#include <QSslSocket>

using namespace NoteShared;

NoteUtils::NoteUtils()
{
}

bool NoteUtils::sendToMail(QWidget *parent, const QString &title, const QString &message)
{
    // get the mail action command
    const QStringList cmd_list = NoteShared::NoteSharedGlobalConfig::mailAction().split(QLatin1Char(' '), Qt::SkipEmptyParts);
    if (cmd_list.isEmpty()) {
        KMessageBox::sorry(parent, i18n("Please configure send mail action."));
        return false;
    }
    KProcess mail;
    for (const QString &cmd : cmd_list) {
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

        if (!port) { // not specified, use default
            port = NoteShared::NoteSharedGlobalConfig::port();
        }

        // Send the note
        auto socket = new QSslSocket;
        socket->connectToHost(host, port);
        auto sender = new NoteShared::NotesNetworkSender(socket);
        sender->setSenderId(NoteShared::NoteSharedGlobalConfig::senderID());
        sender->setNote(title, message); // FIXME: plainText ??
    }
    delete hostDlg;
}

QString NoteUtils::createToolTip(const Akonadi::Item &item)
{
    const auto noteMessage = item.payload<KMime::Message::Ptr>();
    if (!noteMessage) {
        return QString();
    }
    const QString description = QString::fromUtf8(noteMessage->mainBodyPart()->decodedContent());
    const KMime::Headers::Subject *const subject = noteMessage->subject(false);

    const QString realName = subject ? subject->asUnicodeString() : QString();
    const bool isRichText = noteMessage->contentType()->isHTMLText();

    QString tip;
    if (item.hasAttribute<NoteDisplayAttribute>()) {
        const auto attr = item.attribute<NoteDisplayAttribute>();
        if (attr) {
            const QString bckColorName = attr->backgroundColor().name();
            const QString txtColorName = attr->foregroundColor().name();
            const bool textIsLeftToRight = (QApplication::layoutDirection() == Qt::LeftToRight);
            const QString textDirection = textIsLeftToRight ? QStringLiteral("left") : QStringLiteral("right");

            tip = QStringLiteral("<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">");
            tip += QStringLiteral(
                       "<tr>"
                       "<td bgcolor=\"%1\" align=\"%4\" valign=\"middle\">"
                       "<div style=\"color: %2; font-weight: bold;\">"
                       "%3"
                       "</div>"
                       "</td>"
                       "</tr>")
                       .arg(bckColorName, txtColorName, realName.toHtmlEscaped(), textDirection);
            const QString htmlCodeForStandardRow = QStringLiteral(
                "<tr>"
                "<td bgcolor=\"%1\" align=\"left\" valign=\"top\">"
                "<div style=\"color: %2;\">"
                "%3"
                "</div>"
                "</td>"
                "</tr>");

            QString content = description;
            if (!content.trimmed().isEmpty()) {
                tip +=
                    htmlCodeForStandardRow.arg(bckColorName, txtColorName, isRichText ? content : content.replace(QLatin1Char('\n'), QStringLiteral("<br>")));
            }
            tip += QLatin1String(
                "</table"
                "</td>"
                "</tr>");
        }
    }
    return tip;
}

NoteUtils::NoteText NoteUtils::extractNoteText(QString noteText, const QString &titleAddon)
{
    const int pos = noteText.indexOf(QRegularExpression(QStringLiteral("[\r\n]")));
    const QString noteTitle = noteText.left(pos).trimmed() + titleAddon;

    noteText = noteText.mid(pos).trimmed();
    NoteUtils::NoteText noteTextResult;
    noteTextResult.noteText = noteText;
    noteTextResult.noteTitle = noteTitle;
    return noteTextResult;
}
