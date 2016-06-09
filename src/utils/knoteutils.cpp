/*
   Copyright (C) 2013-2016 Montel Laurent <montel@kde.org>

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

#include "knoteutils.h"
#include "attributes/notelockattribute.h"
#include "knotesglobalconfig.h"

#include <KMessageBox>
#include <KLocalizedString>
#include <KProcess>
#include "knotes_debug.h"
#include <KConfigGroup>
#include <QDBusInterface>
#include <QFileInfo>
#include <QStandardPaths>
#include <attributes/notedisplayattribute.h>

void KNoteUtils::updateConfiguration()
{
    QDBusInterface interface(QStringLiteral("org.freedesktop.Akonadi.Agent.akonadi_notes_agent"), QStringLiteral("/NotesAgent"));
    if (interface.isValid()) {
        interface.call(QStringLiteral("configurationChanged"));
    } else {
        qCDebug(KNOTES_LOG) << " Agent not launched";
    }
}

void KNoteUtils::setDefaultValue(Akonadi::Item &item)
{
    NoteShared::NoteDisplayAttribute *attribute =  item.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Item::AddIfMissing);
    attribute->setBackgroundColor(KNotesGlobalConfig::self()->bgColor());
    attribute->setForegroundColor(KNotesGlobalConfig::self()->fgColor());
    attribute->setSize(QSize(KNotesGlobalConfig::self()->width(), KNotesGlobalConfig::self()->height()));
    attribute->setRememberDesktop(KNotesGlobalConfig::self()->rememberDesktop());
    attribute->setTabSize(KNotesGlobalConfig::self()->tabSize());
    attribute->setFont(KNotesGlobalConfig::self()->font());
    attribute->setTitleFont(KNotesGlobalConfig::self()->titleFont());
    attribute->setDesktop(KNotesGlobalConfig::self()->desktop());
    attribute->setIsHidden(KNotesGlobalConfig::self()->hideNote());
    attribute->setPosition(KNotesGlobalConfig::self()->position());
    attribute->setShowInTaskbar(KNotesGlobalConfig::self()->showInTaskbar());
    attribute->setKeepAbove(KNotesGlobalConfig::self()->keepAbove());
    attribute->setKeepBelow(KNotesGlobalConfig::self()->keepBelow());
    attribute->setAutoIndent(KNotesGlobalConfig::self()->autoIndent());
}

