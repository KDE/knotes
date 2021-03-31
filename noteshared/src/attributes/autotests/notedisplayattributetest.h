/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class NoteDisplayAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit NoteDisplayAttributeTest(QObject *parent = nullptr);
    ~NoteDisplayAttributeTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldEqualWhenCloning();
    void shouldHaveType();
};

