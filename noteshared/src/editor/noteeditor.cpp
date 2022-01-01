/*
   SPDX-FileCopyrightText: 2013-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "noteeditor.h"
#include <PimCommon/RichTexteditWithAutoCorrection>

using namespace NoteShared;

NoteEditor::NoteEditor(QWidget *parent)
    : KPIMTextEdit::RichTextEditorWidget(new PimCommon::RichTextEditWithAutoCorrection(parent))
{
}

NoteEditor::~NoteEditor() = default;
