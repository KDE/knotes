/*
   Copyright (C) 2015-2016 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef KNOTESGRANTLEEPRINTTEST_H
#define KNOTESGRANTLEEPRINTTEST_H

#include <QObject>

class KNotesGrantleePrintTest : public QObject
{
    Q_OBJECT
public:
    explicit KNotesGrantleePrintTest(QObject *parent = Q_NULLPTR);
    ~KNotesGrantleePrintTest();

private Q_SLOTS:
    void shouldDefaultValue();
    void shouldReturnEmptyStringWhenNotContentAndNoNotes();
    void shouldReturnEmptyStringWhenAddContentWithoutNotes();

    void shouldDisplayNoteInfo_data();
    void shouldDisplayNoteInfo();
};

#endif // KNOTESGRANTLEEPRINTTEST_H
