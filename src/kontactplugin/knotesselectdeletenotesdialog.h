/*
   Copyright (C) 2013-2020 Laurent Montel <montel@kde.org>

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
#ifndef KNOTESSELECTDELETENOTESDIALOG_H
#define KNOTESSELECTDELETENOTESDIALOG_H

#include <QDialog>
#include <QListWidget>
class KNotesIconViewItem;

class KNotesSelectDeleteNotesListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit KNotesSelectDeleteNotesListWidget(QWidget *parent = nullptr);
    ~KNotesSelectDeleteNotesListWidget();

    void setItems(const QList<KNotesIconViewItem *> &items);
};

class KNotesSelectDeleteNotesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KNotesSelectDeleteNotesDialog(const QList<KNotesIconViewItem *> &items, QWidget *parent = nullptr);
    ~KNotesSelectDeleteNotesDialog();

private:
    void writeConfig();
    void readConfig();
    KNotesSelectDeleteNotesListWidget *mSelectedListWidget = nullptr;
};

#endif // KNOTESSELECTDELETENOTESDIALOG_H
