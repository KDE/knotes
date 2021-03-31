/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QObject>

class KNotesGrantleePrintTest : public QObject
{
    Q_OBJECT
public:
    explicit KNotesGrantleePrintTest(QObject *parent = nullptr);
    ~KNotesGrantleePrintTest();

private Q_SLOTS:
    void shouldReturnEmptyStringWhenNotContentAndNoNotes();
    void shouldReturnEmptyStringWhenAddContentWithoutNotes();

    void shouldDisplayNoteInfo_data();
    void shouldDisplayNoteInfo();
};

