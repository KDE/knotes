/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef KNOTESGRANTLEEPRINTTEST_H
#define KNOTESGRANTLEEPRINTTEST_H

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

#endif // KNOTESGRANTLEEPRINTTEST_H
