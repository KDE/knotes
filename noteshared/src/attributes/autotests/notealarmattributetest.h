/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class NoteAlarmAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit NoteAlarmAttributeTest(QObject *parent = nullptr);
    ~NoteAlarmAttributeTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldAssignValue();
    void shouldCloneAttr();
    void shouldSerializeAttr();
    void shouldHaveType();
};

