/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "noteeditorutils.h"

#include <QLocale>

#include <QChar>
#include <QDateTime>
#include <QTextCursor>
#include <QTextEdit>

using namespace NoteShared;
NoteEditorUtils::NoteEditorUtils()
{
}

void NoteEditorUtils::addCheckmark(QTextCursor &cursor)
{
    static const QChar unicode[] = {0x2713};
    const int size = sizeof(unicode) / sizeof(QChar);
    const int position = cursor.position();
    cursor.movePosition(QTextCursor::StartOfLine);
    const QString checkMark = QString::fromRawData(unicode, size);
    cursor.insertText(checkMark);
    cursor.setPosition(position + checkMark.size());
}

void NoteEditorUtils::insertDate(QTextEdit *editor)
{
    editor->insertPlainText(QLocale().toString(QDateTime::currentDateTime(), QLocale::ShortFormat) + QLatin1Char(' '));
}
