/*
  Copyright (c) 2013-2015 Montel Laurent <montel@kde.org>

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

#include "notesagent.h"
#include "notesmanager.h"
#include "notesagentadaptor.h"
#include "notesharedglobalconfig.h"
#include "notesagentsettings.h"
#include "notesagentsettingsdialog.h"

#include <AgentInstance>
#include <AgentManager>
#include <kdbusconnectionpool.h>
#include <AkonadiCore/changerecorder.h>
#include <AkonadiCore/itemfetchscope.h>
#include <AkonadiCore/session.h>
#include <AttributeFactory>
#include <CollectionFetchScope>

#include <KWindowSystem>
#include <Kdelibs4ConfigMigrator>

#include <QPointer>

NotesAgent::NotesAgent(const QString &id)
    : Akonadi::AgentBase(id),
      mAgentInitialized(false)
{
    Kdelibs4ConfigMigrator migrate(QStringLiteral("notesagent"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("akonadi_notes_agentrc") << QStringLiteral("akonadi_notes_agent.notifyrc"));
    migrate.migrate();

    mNotesManager = new NotesManager(this);
    new NotesAgentAdaptor(this);
    KDBusConnectionPool::threadConnection().registerObject(QStringLiteral("/NotesAgent"), this, QDBusConnection::ExportAdaptors);
    KDBusConnectionPool::threadConnection().registerService(QStringLiteral("org.freedesktop.Akonadi.NotesAgent"));

    setNeedsNetwork(true);

    if (NotesAgentSettings::enabled()) {
        QTimer::singleShot(60 * 1000, this, &NotesAgent::slotStartAgent);
    }
}

NotesAgent::~NotesAgent()
{
}

void NotesAgent::doSetOnline(bool online)
{
    if (mAgentInitialized) {
        if (online) {
            reload();
        } else {
            mNotesManager->stopAll();
        }
    }
}

void NotesAgent::slotStartAgent()
{
    mAgentInitialized = true;
    if (isOnline()) {
        mNotesManager->load();
    }
}

void NotesAgent::reload()
{
    if (NotesAgentSettings::enabled()) {
        mNotesManager->load();
    }
}

void NotesAgent::setEnableAgent(bool enabled)
{
    if (NotesAgentSettings::enabled() == enabled) {
        return;
    }

    NotesAgentSettings::setEnabled(enabled);
    NotesAgentSettings::self()->save();
    if (enabled) {
        mNotesManager->load();
    } else {
        mNotesManager->stopAll();
    }
}

bool NotesAgent::enabledAgent() const
{
    return NotesAgentSettings::enabled();
}

void NotesAgent::configure(WId windowId)
{
    showConfigureDialog((qlonglong)windowId);
}

void NotesAgent::showConfigureDialog(qlonglong windowId)
{
    QPointer<NotesAgentSettingsDialog> dialog = new NotesAgentSettingsDialog;
    if (windowId) {
#ifndef Q_OS_WIN
        KWindowSystem::setMainWindow(dialog, windowId);
#else
        KWindowSystem::setMainWindow(dialog, (HWND)windowId);
#endif
    }
    if (dialog->exec()) {
        mNotesManager->load();
    }
    delete dialog;
}

bool NotesAgent::receiveNotes() const
{
    return NoteShared::NoteSharedGlobalConfig::receiveNotes();
}

void NotesAgent::setReceiveNotes(bool b)
{
    if (NoteShared::NoteSharedGlobalConfig::receiveNotes() != b) {
        NoteShared::NoteSharedGlobalConfig::setReceiveNotes(b);
        NoteShared::NoteSharedGlobalConfig::self()->save();
        mNotesManager->updateNetworkListener();
    }
}

int NotesAgent::port() const
{
    return NoteShared::NoteSharedGlobalConfig::port();
}

void NotesAgent::setPort(int value)
{
    if (value < 0) {
        return;
    }

    if (NoteShared::NoteSharedGlobalConfig::port() != static_cast<uint>(value)) {
        NoteShared::NoteSharedGlobalConfig::setPort(value);
        NoteShared::NoteSharedGlobalConfig::self()->save();
        if (NotesAgentSettings::enabled()) {
            mNotesManager->updateNetworkListener();
        }
    }
}

void NotesAgent::setAlarmCheckInterval(int value)
{
    if (value < 0) {
        return;
    }

    if (NoteShared::NoteSharedGlobalConfig::checkInterval() != static_cast<uint>(value)) {
        NoteShared::NoteSharedGlobalConfig::setCheckInterval(value);
        NoteShared::NoteSharedGlobalConfig::self()->save();
        reload();
    }
}

void NotesAgent::configurationChanged()
{
    NoteShared::NoteSharedGlobalConfig::self()->config()->reparseConfiguration();
    mNotesManager->updateNetworkListener();
}

int NotesAgent::alarmCheckInterval() const
{
    return NoteShared::NoteSharedGlobalConfig::checkInterval();
}

AKONADI_AGENT_MAIN(NotesAgent)

