/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "notelockattributetest.h"
#include "notelockattribute.h"

#include <QTest>

NoteLockAttributeTest::NoteLockAttributeTest(QObject *parent)
    : QObject(parent)
{
}

NoteLockAttributeTest::~NoteLockAttributeTest()
{
}

void NoteLockAttributeTest::shouldHaveType()
{
    NoteShared::NoteLockAttribute attr;
    QCOMPARE(attr.type(), QByteArray("KJotsLockAttribute"));
}

QTEST_GUILESS_MAIN(NoteLockAttributeTest)
