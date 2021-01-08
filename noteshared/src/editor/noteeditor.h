/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef NOTEEDITOR_H
#define NOTEEDITOR_H

#include <KPIMTextEdit/RichTextEditorWidget>

namespace NoteShared {
class NoteEditor : public KPIMTextEdit::RichTextEditorWidget
{
    Q_OBJECT
public:
    explicit NoteEditor(QWidget *parent);
    ~NoteEditor() override;
};
}

#endif // NOTEEDITOR_H
