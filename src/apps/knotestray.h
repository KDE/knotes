/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <KStatusNotifierItem>
#include <QIcon>
class KNotesTray : public KStatusNotifierItem
{
    Q_OBJECT
public:
    explicit KNotesTray(QWidget *parent = nullptr);
    ~KNotesTray() override;

    void updateNumberOfNotes(int value);

private:
    QIcon mIcon;
};

