/*
   Copyright (C) 2013-2020 Laurent Montel <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef KNOTEDISPLAYSETTINGS_H
#define KNOTEDISPLAYSETTINGS_H

#include <QColor>
#include <QFont>
#include <QPoint>

#include "knotes_export.h"
namespace NoteShared {
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

#endif // KNOTEDISPLAYSETTINGS_H
