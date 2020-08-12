/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KNOTEPRINTSELECTTHEMECOMBOBOX_H
#define KNOTEPRINTSELECTTHEMECOMBOBOX_H

#include <QComboBox>
#include "knotes_export.h"

class KNOTES_EXPORT KNotePrintSelectThemeComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit KNotePrintSelectThemeComboBox(QWidget *parent = nullptr);
    ~KNotePrintSelectThemeComboBox();

    Q_REQUIRED_RESULT QString selectedTheme() const;

    void loadThemes();
    void selectDefaultTheme();
};

#endif // KNOTEPRINTSELECTTHEMECOMBOBOX_H
