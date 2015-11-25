/*
  Copyright (c) 2013-2015 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "noteeditorutils.h"

#include <QLocale>

#include <QChar>
#include <QTextCursor>
#include <QTextEdit>
#include <QDateTime>

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
