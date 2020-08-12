/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef NOTESAGENTNOTEDIALOG_H
#define NOTESAGENTNOTEDIALOG_H

#include <QDialog>
#include <Item>
class QLineEdit;
class KJob;
namespace KPIMTextEdit {
class RichTextEditorWidget;
}
class NotesAgentNoteDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NotesAgentNoteDialog(QWidget *parent = nullptr);
    ~NotesAgentNoteDialog();

    void setNoteId(Akonadi::Item::Id id);

private Q_SLOTS:
    void slotFetchItem(KJob *job);

private:
    void readConfig();
    void writeConfig();
    KPIMTextEdit::RichTextEditorWidget *mNote = nullptr;
    QLineEdit *mSubject = nullptr;
};

#endif // NOTESAGENTNOTEDIALOG_H
