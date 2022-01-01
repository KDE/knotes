/*
   SPDX-FileCopyrightText: 2013-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QObject>

class NoteEditorUtilsTest : public QObject
{
    Q_OBJECT
public:
    explicit NoteEditorUtilsTest(QObject *parent = nullptr);
    ~NoteEditorUtilsTest() override;

private Q_SLOTS:
    void testAddCheckmark();
    void testAddCheckmark_data();
};

