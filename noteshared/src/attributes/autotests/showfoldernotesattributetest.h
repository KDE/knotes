/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SHOWFOLDERNOTESATTRIBUTETEST_H
#define SHOWFOLDERNOTESATTRIBUTETEST_H

#include <QObject>

class ShowFolderNotesAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit ShowFolderNotesAttributeTest(QObject *parent = nullptr);
    ~ShowFolderNotesAttributeTest();
private Q_SLOTS:
    void shouldHaveType();
};

#endif // SHOWFOLDERNOTESATTRIBUTETEST_H
