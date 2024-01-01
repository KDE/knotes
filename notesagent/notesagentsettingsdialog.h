/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QDialog>

class KNotifyConfigWidget;
namespace NoteShared
{
class NoteNetworkConfigWidget;
}
class NotesAgentSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NotesAgentSettingsDialog(QWidget *parent = nullptr);
    ~NotesAgentSettingsDialog() override;

private:
    void slotOkClicked();
    void writeConfig();
    void readConfig();
    KNotifyConfigWidget *const mNotify;
    NoteShared::NoteNetworkConfigWidget *const mNetworkConfig;
};
