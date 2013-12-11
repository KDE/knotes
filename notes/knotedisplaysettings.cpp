/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

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


#include "knotedisplaysettings.h"
#include "noteshared/attributes/notedisplayattribute.h"
#include "knotesglobalconfig.h"


KNoteDisplaySettings::KNoteDisplaySettings(NoteShared::NoteDisplayAttribute *attr)
    : mDisplayAttribute(attr)
{
}

KNoteDisplaySettings::~KNoteDisplaySettings()
{

}

void KNoteDisplaySettings::setDisplayAttribute(NoteShared::NoteDisplayAttribute *attr)
{
    mDisplayAttribute = attr;
}

NoteShared::NoteDisplayAttribute *KNoteDisplaySettings::displayAttribute() const
{
    return mDisplayAttribute;
}

QColor KNoteDisplaySettings::backgroundColor() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->backgroundColor();
    else
        return KNotesGlobalConfig::self()->bgColor();
}

QColor KNoteDisplaySettings::foregroundColor() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->foregroundColor();
    else
        return KNotesGlobalConfig::self()->fgColor();
}

QSize KNoteDisplaySettings::size() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->size();
    else
        return QSize(KNotesGlobalConfig::self()->width(), KNotesGlobalConfig::self()->height());
}

bool KNoteDisplaySettings::rememberDesktop() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->rememberDesktop();
    else
        return KNotesGlobalConfig::self()->rememberDesktop();
}

int KNoteDisplaySettings::tabSize() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->tabSize();
    else
        return KNotesGlobalConfig::self()->tabSize();
}

QFont KNoteDisplaySettings::font() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->font();
    else
        return KNotesGlobalConfig::self()->font();
}

QFont KNoteDisplaySettings::titleFont() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->titleFont();
    else
        return KNotesGlobalConfig::self()->titleFont();
}

int KNoteDisplaySettings::desktop() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->desktop();
    else
        return KNotesGlobalConfig::self()->desktop();
}

bool KNoteDisplaySettings::isHidden() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->isHidden();
    else
        return KNotesGlobalConfig::self()->hideNote();
}

QPoint KNoteDisplaySettings::position() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->position();
    }
    else
        return KNotesGlobalConfig::self()->position();
}

bool KNoteDisplaySettings::showInTaskbar() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->showInTaskbar();
    else
        return KNotesGlobalConfig::self()->showInTaskbar();
}

bool KNoteDisplaySettings::keepAbove() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->keepAbove();
    else
        return KNotesGlobalConfig::self()->keepAbove();
}

bool KNoteDisplaySettings::keepBelow() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->keepBelow();
    else
        return KNotesGlobalConfig::self()->keepBelow();
}

bool KNoteDisplaySettings::autoIndent() const
{
    if (mDisplayAttribute)
        return mDisplayAttribute->autoIndent();
    else
        return KNotesGlobalConfig::self()->autoIndent();
}
