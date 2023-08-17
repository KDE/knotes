/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <TextCustomEditor/RichTextEditorWidget>

namespace NoteShared
{
class NoteEditor : public TextCustomEditor::RichTextEditorWidget
{
    Q_OBJECT
public:
    explicit NoteEditor(QWidget *parent);
    ~NoteEditor() override;
};
}
