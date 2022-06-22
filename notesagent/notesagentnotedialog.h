/*
   SPDX-FileCopyrightText: 2013-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <Akonadi/Item>
#include <QDialog>
class QLineEdit;
class KJob;
namespace KPIMTextEdit
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

private Q_SLOTS:
    void slotFetchItem(KJob *job);

private:
    void readConfig();
    void writeConfig();
    KPIMTextEdit::RichTextEditorWidget *const mNote;
    QLineEdit *const mSubject;
};
