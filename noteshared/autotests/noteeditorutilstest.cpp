/*
  Copyright (c) 2015 Montel Laurent <montel@kde.org>

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

#include "noteeditorutilstest.h"
#include <QTextCursor>
#include <QTextDocument>
#include <qtest.h>

#include <editor/noteeditorutils.h>

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
    QTest::newRow("standard") <<  QStringLiteral("foo") << 0;
    QTest::newRow("emptyline") <<  QStringLiteral("") << 0;
}

void NoteEditorUtilsTest::testAddCheckmark()
{
    QFETCH(QString, input);
    QFETCH(int, position);

    static const QChar unicode[] = {0x2713};
    const int size = sizeof(unicode) / sizeof(QChar);
    const QString checkMark = QString::fromRawData(unicode, size);

    NoteShared::NoteEditorUtils noteUtils;
    QTextDocument *document = new QTextDocument(this);
    document->setPlainText(input);
    QTextCursor textCursor(document);
    if (position < 1) {
        position = 0;
    }
    textCursor.setPosition(position);

    document->setPlainText(input);

    noteUtils.addCheckmark(textCursor);
    QCOMPARE(textCursor.document()->toPlainText(), QString(checkMark + input));
    //QCOMPARE(textCursor.position(), position + checkMark.size());
    delete document;
}

QTEST_MAIN(NoteEditorUtilsTest)
