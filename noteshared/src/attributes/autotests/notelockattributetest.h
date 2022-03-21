/*
   SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class NoteLockAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit NoteLockAttributeTest(QObject *parent = nullptr);
    ~NoteLockAttributeTest() override;
private Q_SLOTS:
    void shouldHaveType();
};
