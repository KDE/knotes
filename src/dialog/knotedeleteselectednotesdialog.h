/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef KNOTEDELETESELECTEDNOTESDIALOG_H
#define KNOTEDELETESELECTEDNOTESDIALOG_H

#include <QDialog>
#include <AkonadiCore/Item>
class QPushButton;
namespace NoteShared {
class NoteListWidget;
}

class KNoteDeleteSelectedNotesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KNoteDeleteSelectedNotesDialog(QWidget *parent = nullptr);
    ~KNoteDeleteSelectedNotesDialog() override;

    void setNotes(const Akonadi::Item::List &notes);

    Akonadi::Item::List selectedNotes() const;

private Q_SLOTS:
    void slotDeleteNoteSelectionChanged();
private:
    void readConfig();
    void writeConfig();
    NoteShared::NoteListWidget *mNoteList = nullptr;
    QPushButton *mOkButton = nullptr;
};

#endif // KNOTEDELETESELECTEDNOTESDIALOG_H
