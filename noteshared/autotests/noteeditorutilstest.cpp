/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "noteeditorutilstest.h"
#include "editor/noteeditorutils.h"

#include <QTextCursor>
#include <QTextDocument>

#include <QTest>

NoteEditorUtilsTest::NoteEditorUtilsTest(QObject *parent)
    : QObject(parent)
{
}

NoteEditorUtilsTest::~NoteEditorUtilsTest()
{
}

void NoteEditorUtilsTest::testAddCheckmark_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("position");
    QTest::newRow("standard") << QStringLiteral("foo") << 0;
    QTest::newRow("emptyline") << QString() << 0;
}

void NoteEditorUtilsTest::testAddCheckmark()
{
    QFETCH(QString, input);
    QFETCH(int, position);

    static const QChar unicode[] = {0x2713};
    const int size = sizeof(unicode) / sizeof(QChar);
    const QString checkMark = QString::fromRawData(unicode, size);

    NoteShared::NoteEditorUtils noteUtils;
    auto document = new QTextDocument(this);
    document->setPlainText(input);
    QTextCursor textCursor(document);
    if (position < 1) {
        position = 0;
    }
    textCursor.setPosition(position);

    document->setPlainText(input);

    noteUtils.addCheckmark(textCursor);
    QCOMPARE(textCursor.document()->toPlainText(), QString(checkMark + input));
    // QCOMPARE(textCursor.position(), position + checkMark.size());
    delete document;
}

QTEST_MAIN(NoteEditorUtilsTest)
