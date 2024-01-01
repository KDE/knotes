/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <Akonadi/Item>
#include <QDialog>
class QLineEdit;
class KJob;
namespace TextCustomEditor
{
class RichTextEditorWidget;
}
class NotesAgentNoteDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NotesAgentNoteDialog(QWidget *parent = nullptr);
    ~NotesAgentNoteDialog() override;

    void setNoteId(Akonadi::Item::Id id);

private:
    void slotFetchItem(KJob *job);
    void readConfig();
    void writeConfig();
    TextCustomEditor::RichTextEditorWidget *const mNote;
    QLineEdit *const mSubject;
};
