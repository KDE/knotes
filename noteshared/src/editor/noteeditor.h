/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <KPIMTextEdit/RichTextEditorWidget>

namespace NoteShared
{
class NoteEditor : public KPIMTextEdit::RichTextEditorWidget
{
    Q_OBJECT
public:
    explicit NoteEditor(QWidget *parent);
    ~NoteEditor() override;
};
}

