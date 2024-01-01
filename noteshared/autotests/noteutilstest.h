/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class NoteUtilsTest : public QObject
{
    Q_OBJECT
public:
    explicit NoteUtilsTest(QObject *parent = nullptr);
    ~NoteUtilsTest() override = default;

private Q_SLOTS:
    void testExtractNoteText_data();
    void testExtractNoteText();
};
