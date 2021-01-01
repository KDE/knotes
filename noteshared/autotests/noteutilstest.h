/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef NOTEUTILSTEST_H
#define NOTEUTILSTEST_H

#include <QObject>

class NoteUtilsTest : public QObject
{
    Q_OBJECT
public:
    explicit NoteUtilsTest(QObject *parent = nullptr);
    ~NoteUtilsTest() = default;

private Q_SLOTS:
    void testExtractNoteText_data();
    void testExtractNoteText();
};

#endif // NOTEUTILSTEST_H
