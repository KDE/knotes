/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <AkonadiWidgets/CollectionDialog>

namespace NoteShared
{
class SelectedNotefolderDialog : public Akonadi::CollectionDialog
{
    Q_OBJECT
public:
    explicit SelectedNotefolderDialog(QWidget *parent = nullptr);
    ~SelectedNotefolderDialog() override;

private:
    void readConfig();
    void writeConfig();
};
}

