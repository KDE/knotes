/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef NOTESAGENTSETTINGSDIALOG_H
#define NOTESAGENTSETTINGSDIALOG_H

#include <QDialog>

class KNotifyConfigWidget;
namespace NoteShared {
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

#endif // NOTESAGENTSETTINGSDIALOG_H
