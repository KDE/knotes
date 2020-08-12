/*
   SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef NOTELOCKATTRIBUTETEST_H
#define NOTELOCKATTRIBUTETEST_H

#include <QObject>

class NoteLockAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit NoteLockAttributeTest(QObject *parent = nullptr);
    ~NoteLockAttributeTest();
private Q_SLOTS:
    void shouldHaveType();
};

#endif // NOTELOCKATTRIBUTETEST_H
