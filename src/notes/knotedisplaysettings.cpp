/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knotedisplaysettings.h"
#include "attributes/notedisplayattribute.h"
#include "knotesglobalconfig.h"

KNoteDisplaySettings::KNoteDisplaySettings(NoteShared::NoteDisplayAttribute *attr)
    : mDisplayAttribute(attr)
{
}

KNoteDisplaySettings::~KNoteDisplaySettings()
{
}

void KNoteDisplaySettings::setDisplayAttribute(const NoteShared::NoteDisplayAttribute *attr)
{
    mDisplayAttribute = attr;
}

const NoteShared::NoteDisplayAttribute *KNoteDisplaySettings::displayAttribute() const
{
    return mDisplayAttribute;
}

QColor KNoteDisplaySettings::backgroundColor() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->backgroundColor();
    } else {
        return KNotesGlobalConfig::self()->bgColor();
    }
}

QColor KNoteDisplaySettings::foregroundColor() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->foregroundColor();
    } else {
        return KNotesGlobalConfig::self()->fgColor();
    }
}

QSize KNoteDisplaySettings::size() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->size();
    } else {
        return QSize(KNotesGlobalConfig::self()->width(), KNotesGlobalConfig::self()->height());
    }
}

bool KNoteDisplaySettings::rememberDesktop() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->rememberDesktop();
    } else {
        return KNotesGlobalConfig::self()->rememberDesktop();
    }
}

int KNoteDisplaySettings::tabSize() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->tabSize();
    } else {
        return KNotesGlobalConfig::self()->tabSize();
    }
}

QFont KNoteDisplaySettings::font() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->font();
    } else {
        return KNotesGlobalConfig::self()->font();
    }
}

QFont KNoteDisplaySettings::titleFont() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->titleFont();
    } else {
        return KNotesGlobalConfig::self()->titleFont();
    }
}

int KNoteDisplaySettings::desktop() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->desktop();
    } else {
        return KNotesGlobalConfig::self()->desktop();
    }
}

bool KNoteDisplaySettings::isHidden() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->isHidden();
    } else {
        return KNotesGlobalConfig::self()->hideNote();
    }
}

QPoint KNoteDisplaySettings::position() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->position();
    } else {
        return KNotesGlobalConfig::self()->position();
    }
}

bool KNoteDisplaySettings::showInTaskbar() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->showInTaskbar();
    } else {
        return KNotesGlobalConfig::self()->showInTaskbar();
    }
}

bool KNoteDisplaySettings::keepAbove() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->keepAbove();
    } else {
        return KNotesGlobalConfig::self()->keepAbove();
    }
}

bool KNoteDisplaySettings::keepBelow() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->keepBelow();
    } else {
        return KNotesGlobalConfig::self()->keepBelow();
    }
}

bool KNoteDisplaySettings::autoIndent() const
{
    if (mDisplayAttribute) {
        return mDisplayAttribute->autoIndent();
    } else {
        return KNotesGlobalConfig::self()->autoIndent();
    }
}
