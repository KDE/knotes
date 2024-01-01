/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "notelockattributetest.h"
#include "notelockattribute.h"

#include <QTest>

NoteLockAttributeTest::NoteLockAttributeTest(QObject *parent)
    : QObject(parent)
{
}

NoteLockAttributeTest::~NoteLockAttributeTest() = default;

void NoteLockAttributeTest::shouldHaveType()
{
    NoteShared::NoteLockAttribute attr;
    QCOMPARE(attr.type(), QByteArray("KJotsLockAttribute"));
}

QTEST_GUILESS_MAIN(NoteLockAttributeTest)

#include "moc_notelockattributetest.cpp"
