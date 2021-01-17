/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "notealarmattributetest.h"
#include "notealarmattribute.h"

#include <QTest>

NoteAlarmAttributeTest::NoteAlarmAttributeTest(QObject *parent)
    : QObject(parent)
{
}

NoteAlarmAttributeTest::~NoteAlarmAttributeTest()
{
}

void NoteAlarmAttributeTest::shouldHaveDefaultValue()
{
    NoteShared::NoteAlarmAttribute attr;
    QVERIFY(!attr.dateTime().isValid());
}

void NoteAlarmAttributeTest::shouldAssignValue()
{
    const QDateTime dt = QDateTime::currentDateTime();
    NoteShared::NoteAlarmAttribute attr;
    attr.setDateTime(dt);
    QCOMPARE(dt, attr.dateTime());
}

void NoteAlarmAttributeTest::shouldCloneAttr()
{
    const QDateTime dt = QDateTime::currentDateTime();
    NoteShared::NoteAlarmAttribute attr;
    attr.setDateTime(dt);
    NoteShared::NoteAlarmAttribute *result = attr.clone();
    QVERIFY(attr == *result);
    delete result;
}

void NoteAlarmAttributeTest::shouldSerializeAttr()
{
    const QDateTime dt = QDateTime::currentDateTime();
    NoteShared::NoteAlarmAttribute attr;
    attr.setDateTime(dt);
    const QByteArray ba = attr.serialized();
    NoteShared::NoteAlarmAttribute result;
    result.deserialize(ba);
    QVERIFY(attr == result);
}

void NoteAlarmAttributeTest::shouldHaveType()
{
    NoteShared::NoteAlarmAttribute attr;
    QCOMPARE(attr.type(), QByteArray("NoteAlarmAttribute"));
}

QTEST_GUILESS_MAIN(NoteAlarmAttributeTest)
