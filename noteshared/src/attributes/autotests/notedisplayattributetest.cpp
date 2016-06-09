/*
   Copyright (C) 2014-2016 Montel Laurent <montel@kde.org>

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

#include "notedisplayattributetest.h"
#include <qtest.h>
#include <QFontDatabase>
#include "../notedisplayattribute.h"

NoteDisplayAttributeTest::NoteDisplayAttributeTest(QObject *parent)
    : QObject(parent)
{

}

NoteDisplayAttributeTest::~NoteDisplayAttributeTest()
{

}

void NoteDisplayAttributeTest::shouldHaveDefaultValue()
{
    NoteShared::NoteDisplayAttribute attribute;
    QVERIFY(attribute.autoIndent());
    QVERIFY(!attribute.keepBelow());
    QVERIFY(!attribute.keepAbove());
    QVERIFY(!attribute.showInTaskbar());
    QVERIFY(!attribute.isHidden());
    QCOMPARE(attribute.backgroundColor(), QColor(Qt::yellow));
    QCOMPARE(attribute.foregroundColor(), QColor(Qt::black));
    QVERIFY(attribute.rememberDesktop());
    QCOMPARE(attribute.tabSize(), 4);
    QCOMPARE(attribute.font(), QFontDatabase::systemFont(QFontDatabase::GeneralFont));
    QCOMPARE(attribute.titleFont(), QFontDatabase::systemFont(QFontDatabase::TitleFont));
    QCOMPARE(attribute.size(), QSize(300, 300));
    QCOMPARE(attribute.desktop(), -10);
    QCOMPARE(attribute.position(), QPoint(-10000, -10000));
}

void NoteDisplayAttributeTest::shouldEqualWhenCloning()
{
    NoteShared::NoteDisplayAttribute attribute;
    NoteShared::NoteDisplayAttribute *attr2 = attribute.clone();
    QVERIFY(attribute == *attr2);
    delete attr2;

    attribute.setBackgroundColor(QColor("#454545"));
    attribute.setForegroundColor(QColor("#454545"));
    attribute.setSize(QSize(50, 70));
    attribute.setRememberDesktop(false);
    attribute.setTabSize(8);
    attribute.setFont(QFont());
    attribute.setTitleFont(QFont());
    attribute.setDesktop(7);
    attribute.setIsHidden(true);
    attribute.setPosition(QPoint(7, 8));
    attribute.setShowInTaskbar(false);
    attribute.setKeepAbove(false);
    attribute.setKeepBelow(false);
    attribute.setAutoIndent(false);
    attr2 = attribute.clone();
    QVERIFY(attribute == *attr2);
    delete attr2;
}

void NoteDisplayAttributeTest::shouldHaveType()
{
    NoteShared::NoteDisplayAttribute attribute;
    QCOMPARE(attribute.type(), QByteArray("NoteDisplayAttribute"));
}

QTEST_MAIN(NoteDisplayAttributeTest)
