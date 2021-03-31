/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <KListWidgetSearchLine>

class KNotesListWidgetSearchLine : public KListWidgetSearchLine
{
    Q_OBJECT
public:
    explicit KNotesListWidgetSearchLine(QWidget *parent = nullptr);
    ~KNotesListWidgetSearchLine() override;

    void updateClickMessage(const QString &shortcutStr);

protected:
    bool itemMatches(const QListWidgetItem *item, const QString &s) const override;
};

