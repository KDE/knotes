/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QColor>
#include <QFont>
#include <QPoint>

#include "knotes_export.h"
namespace NoteShared
{
class NoteDisplayAttribute;
}

class KNOTES_EXPORT KNoteDisplaySettings
{
public:
    explicit KNoteDisplaySettings(NoteShared::NoteDisplayAttribute *attr = nullptr);
    ~KNoteDisplaySettings();

    void setDisplayAttribute(const NoteShared::NoteDisplayAttribute *attr);
    const NoteShared::NoteDisplayAttribute *displayAttribute() const;
    [[nodiscard]] QColor backgroundColor() const;

    [[nodiscard]] QColor foregroundColor() const;
    [[nodiscard]] QSize size() const;
    [[nodiscard]] bool rememberDesktop() const;
    [[nodiscard]] int tabSize() const;
    [[nodiscard]] QFont font() const;
    [[nodiscard]] QFont titleFont() const;
    [[nodiscard]] int desktop() const;
    [[nodiscard]] bool isHidden() const;
    [[nodiscard]] QPoint position() const;
    [[nodiscard]] bool showInTaskbar() const;
    [[nodiscard]] bool keepAbove() const;
    [[nodiscard]] bool keepBelow() const;
    [[nodiscard]] bool autoIndent() const;

private:
    const NoteShared::NoteDisplayAttribute *mDisplayAttribute = nullptr;
};
