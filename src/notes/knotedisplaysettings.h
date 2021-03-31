/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

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
    Q_REQUIRED_RESULT QColor backgroundColor() const;

    Q_REQUIRED_RESULT QColor foregroundColor() const;
    Q_REQUIRED_RESULT QSize size() const;
    Q_REQUIRED_RESULT bool rememberDesktop() const;
    Q_REQUIRED_RESULT int tabSize() const;
    Q_REQUIRED_RESULT QFont font() const;
    Q_REQUIRED_RESULT QFont titleFont() const;
    Q_REQUIRED_RESULT int desktop() const;
    Q_REQUIRED_RESULT bool isHidden() const;
    Q_REQUIRED_RESULT QPoint position() const;
    Q_REQUIRED_RESULT bool showInTaskbar() const;
    Q_REQUIRED_RESULT bool keepAbove() const;
    Q_REQUIRED_RESULT bool keepBelow() const;
    Q_REQUIRED_RESULT bool autoIndent() const;

private:
    const NoteShared::NoteDisplayAttribute *mDisplayAttribute = nullptr;
};

