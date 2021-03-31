/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

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
    ~NotesAgentSettingsDialog();

private Q_SLOTS:
    void slotOkClicked();

private:
    void writeConfig();
    void readConfig();
    KNotifyConfigWidget *mNotify = nullptr;
    NoteShared::NoteNetworkConfigWidget *mNetworkConfig = nullptr;
};

