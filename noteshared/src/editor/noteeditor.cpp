/*
   SPDX-FileCopyrightText: 2013-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "noteeditor.h"
#include "widget/richtexteditwithautocorrection.h"

using namespace NoteShared;

NoteEditor::NoteEditor(QWidget *parent)
    : KPIMTextEdit::RichTextEditorWidget(new NoteShared::RichTextEditWithAutoCorrection(parent))
{
}

NoteEditor::~NoteEditor() = default;
