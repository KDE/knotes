/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QDialog>
class KActionCollection;
class KShortcutsEditor;
class KNotesKeyDialog : public QDialog
{
public:
    explicit KNotesKeyDialog(KActionCollection *globals, QWidget *parent);
    ~KNotesKeyDialog() override;

    void insert(KActionCollection *actions);

    void save();

private:
    void readConfig();
    void writeConfig();
    KShortcutsEditor *m_keyChooser = nullptr;
};

