/*
   SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class ShowFolderNotesAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit ShowFolderNotesAttributeTest(QObject *parent = nullptr);
    ~ShowFolderNotesAttributeTest() override;
private Q_SLOTS:
    void shouldHaveType();
};

