/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
class QTextCursor;
class QTextEdit;
namespace NoteShared
{
class NOTESHARED_EXPORT NoteEditorUtils
{
public:
    NoteEditorUtils();
    void addCheckmark(QTextCursor &cursor);
    void insertDate(QTextEdit *editor);
};
}
