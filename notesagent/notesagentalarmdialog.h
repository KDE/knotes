/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef NOTESAGENTALARMDIALOG_H
#define NOTESAGENTALARMDIALOG_H
#include <QDialog>
#include <Item>
class QListWidget;
class QLabel;
class QListWidgetItem;
namespace NoteShared {
class NoteListWidget;
}
class KJob;
class NotesAgentAlarmDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NotesAgentAlarmDialog(QWidget *parent = nullptr);
    ~NotesAgentAlarmDialog();

    void addListAlarm(const Akonadi::Item::List &lstAlarm);
    void removeAlarm(const Akonadi::Item &note);

private Q_SLOTS:
    void slotItemDoubleClicked(QListWidgetItem *item);
    void slotCustomContextMenuRequested(const QPoint &pos);
    void slotShowNote();
    void slotRemoveAlarm();
    void slotFetchItem(KJob *job);
    void slotModifyItem(KJob *job);
    void slotModifyAlarm();
    void slotFetchAlarmItem(KJob *job);

private:
    void readConfig();
    void writeConfig();
    NoteShared::NoteListWidget *mListWidget = nullptr;
    QLabel *mCurrentDateTime = nullptr;
};

#endif // NOTESAGENTALARMDIALOG_H
