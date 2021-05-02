/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knoteutils.h"
#include "attributes/notedisplayattribute.h"
#include "knotes_debug.h"
#include "knotesglobalconfig.h"

#include <AkonadiCore/ServerManager>

#include <QDBusInterface>

void KNoteUtils::updateConfiguration()
{
    QString service = Akonadi::ServerManager::agentServiceName(Akonadi::ServerManager::Agent, QStringLiteral("akonadi_notes_agent"));

    QDBusInterface interface(service, QStringLiteral("/NotesAgent"));
    if (interface.isValid()) {
        interface.call(QStringLiteral("configurationChanged"));
    } else {
        qCDebug(KNOTES_LOG) << " Agent not launched";
    }
}

void KNoteUtils::setDefaultValue(Akonadi::Item &item)
{
    auto attribute = item.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Item::AddIfMissing);

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
