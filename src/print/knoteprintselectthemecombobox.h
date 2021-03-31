/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "knotes_export.h"
#include <QComboBox>

class KNOTES_EXPORT KNotePrintSelectThemeComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit KNotePrintSelectThemeComboBox(QWidget *parent = nullptr);
    ~KNotePrintSelectThemeComboBox() override;

    Q_REQUIRED_RESULT QString selectedTheme() const;

    void loadThemes();
    void selectDefaultTheme();
};

