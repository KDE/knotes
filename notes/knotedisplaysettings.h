/*
  Copyright (c) 2013, 2014 Montel Laurent <montel.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef KNOTEDISPLAYSETTINGS_H
#define KNOTEDISPLAYSETTINGS_H

#include <QColor>
#include <QFont>
#include <QPoint>

namespace NoteShared {
class NoteDisplayAttribute;
}

class KNoteDisplaySettings
{
public:
    KNoteDisplaySettings(NoteShared::NoteDisplayAttribute *attr=0);
    ~KNoteDisplaySettings();

    void setDisplayAttribute(NoteShared::NoteDisplayAttribute* attr);
    NoteShared::NoteDisplayAttribute *displayAttribute() const;
    QColor backgroundColor() const;

    QColor foregroundColor() const;
    QSize size() const;
    bool rememberDesktop() const;
    int tabSize() const;
    QFont font() const;
    QFont titleFont() const;
    int desktop() const;
    bool isHidden() const;
    QPoint position() const;
    bool showInTaskbar() const;
    bool keepAbove() const;
    bool keepBelow() const;
    bool autoIndent() const;

private:
    NoteShared::NoteDisplayAttribute *mDisplayAttribute;
};

#endif // KNOTEDISPLAYSETTINGS_H
