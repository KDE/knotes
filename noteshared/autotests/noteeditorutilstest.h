/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef NOTEEDITORUTILSTEST_H
#define NOTEEDITORUTILSTEST_H

#include <QObject>

class NoteEditorUtilsTest : public QObject
{
    Q_OBJECT
public:
    explicit NoteEditorUtilsTest(QObject *parent = nullptr);
    ~NoteEditorUtilsTest();

private Q_SLOTS:
    void testAddCheckmark();
    void testAddCheckmark_data();
};

#endif // NOTEEDITORUTILSTEST_H
