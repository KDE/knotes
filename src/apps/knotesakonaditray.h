/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <KStatusNotifierItem>
#include <QIcon>
class KNotesAkonadiTray : public KStatusNotifierItem
{
    Q_OBJECT
public:
    explicit KNotesAkonadiTray(QWidget *parent = nullptr);
    ~KNotesAkonadiTray() override;

    void updateNumberOfNotes(int number);

private:
    void slotGeneralPaletteChanged();
    const QIcon mIcon;
    QColor mTextColor;
};

