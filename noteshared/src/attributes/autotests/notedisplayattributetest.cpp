/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "notedisplayattributetest.h"
#include "notedisplayattribute.h"

#include <QFontDatabase>

#include <QTest>

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
    //@krazy:cond=qenums because everyone expects yellow postit notes
    QCOMPARE(attribute.backgroundColor(), QColor(Qt::yellow));
    QCOMPARE(attribute.foregroundColor(), QColor(Qt::black));
    //@krazy:endcond=qenums
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

QTEST_GUILESS_MAIN(NoteDisplayAttributeTest)
