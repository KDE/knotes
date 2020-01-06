/*
   Copyright (C) 2020 Laurent Montel <montel@kde.org>

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

#include "noteutilstest.h"
#include "noteutils.h"
#include <QTest>
QTEST_MAIN(NoteUtilsTest)

NoteUtilsTest::NoteUtilsTest(QObject *parent)
    : QObject(parent)
{

}

void NoteUtilsTest::testExtractNoteText_data()
{
    QTest::addColumn<QString>("titleAddons");
    QTest::addColumn<QString>("noteText");
    QTest::addColumn<QString>("resultNoteTitle");
    QTest::addColumn<QString>("resultNoteText");
    QTest::newRow("empty") <<  QString() << QString() <<  QString() << QString();
    QTest::newRow("test1") <<  QStringLiteral("foo") << QStringLiteral("bla\r\nfoo") << QStringLiteral("blafoo") <<  QStringLiteral("foo");
    QTest::newRow("test2") <<  QString() << QStringLiteral("bla\r\nfoo") << QStringLiteral("bla") <<  QStringLiteral("foo");
    QTest::newRow("test3") <<  QString() << QStringLiteral("blafoo") << QStringLiteral("blafoo") <<  QStringLiteral("blafoo");
}

void NoteUtilsTest::testExtractNoteText()
{
    QFETCH(QString, titleAddons);
    QFETCH(QString, noteText);
    QFETCH(QString, resultNoteTitle);
    QFETCH(QString, resultNoteText);

    NoteShared::NoteUtils noteUtils;
    NoteShared::NoteUtils::NoteText result = noteUtils.extractNoteText(noteText, titleAddons);
    QCOMPARE(result.noteText, resultNoteText);
    QCOMPARE(result.noteTitle, resultNoteTitle);
}
