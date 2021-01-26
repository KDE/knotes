/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "noteutilstest.h"
#include "noteutils.h"
#include <QTest>
QTEST_GUILESS_MAIN(NoteUtilsTest)

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
    QTest::newRow("empty") << QString() << QString() << QString() << QString();
    QTest::newRow("test1") << QStringLiteral("foo") << QStringLiteral("bla\r\nfoo") << QStringLiteral("blafoo") << QStringLiteral("foo");
    QTest::newRow("test2") << QString() << QStringLiteral("bla\r\nfoo") << QStringLiteral("bla") << QStringLiteral("foo");
    QTest::newRow("test3") << QString() << QStringLiteral("blafoo") << QStringLiteral("blafoo") << QStringLiteral("blafoo");
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
