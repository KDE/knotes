/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

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

