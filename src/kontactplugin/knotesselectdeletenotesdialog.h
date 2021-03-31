/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QDialog>
#include <QListWidget>
class KNotesIconViewItem;

class KNotesSelectDeleteNotesListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit KNotesSelectDeleteNotesListWidget(QWidget *parent = nullptr);
    ~KNotesSelectDeleteNotesListWidget() override;

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

