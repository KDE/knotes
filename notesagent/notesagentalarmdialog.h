/*
   Copyright (C) 2013-2016 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef NOTESAGENTALARMDIALOG_H
#define NOTESAGENTALARMDIALOG_H
#include <QDialog>
#include <Item>
class QListWidget;
class QLabel;
class QListWidgetItem;
namespace NoteShared
{
class NoteListWidget;
}
class KJob;
class NotesAgentAlarmDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NotesAgentAlarmDialog(QWidget *parent = Q_NULLPTR);
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
    NoteShared::NoteListWidget *mListWidget;
    QLabel *mCurrentDateTime;
};

#endif // NOTESAGENTALARMDIALOG_H
