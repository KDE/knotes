/*
   SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "showfoldernotesattributetest.h"
#include "showfoldernotesattribute.h"

#include <QTest>

ShowFolderNotesAttributeTest::ShowFolderNotesAttributeTest(QObject *parent)
    : QObject(parent)
{
}

ShowFolderNotesAttributeTest::~ShowFolderNotesAttributeTest() = default;

void ShowFolderNotesAttributeTest::shouldHaveType()
{
    NoteShared::ShowFolderNotesAttribute attr;
    QCOMPARE(attr.type(), QByteArray("showfoldernotesattribute"));
}

QTEST_GUILESS_MAIN(ShowFolderNotesAttributeTest)
