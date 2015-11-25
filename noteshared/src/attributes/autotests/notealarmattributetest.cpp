/*
  Copyright (c) 2014-2015 Montel Laurent <montel@kde.org>

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

#include "notealarmattributetest.h"
#include "../notealarmattribute.h"
#include <qtest.h>
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

QTEST_MAIN(NoteAlarmAttributeTest)
