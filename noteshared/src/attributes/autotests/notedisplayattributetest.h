/*
   SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class NoteDisplayAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit NoteDisplayAttributeTest(QObject *parent = nullptr);
    ~NoteDisplayAttributeTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldEqualWhenCloning();
    void shouldHaveType();
};

