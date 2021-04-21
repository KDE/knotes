/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "notesagent.h"
#include "notesagentadaptor.h"
#include "notesagentsettings.h"
#include "notesagentsettingsdialog.h"
#include "notesharedglobalconfig.h"
#include "notesmanager.h"

#include <AkonadiCore/ServerManager>

#include <kcoreaddons_version.h>
#if KCOREADDONS_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <Kdelibs4ConfigMigrator>
#endif

#include <KWindowSystem>
#include <QDBusConnection>

NotesAgent::NotesAgent(const QString &id)
    : Akonadi::AgentBase(id)
{
#if KCOREADDONS_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Kdelibs4ConfigMigrator migrate(QStringLiteral("notesagent"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("akonadi_notes_agentrc") << QStringLiteral("akonadi_notes_agent.notifyrc"));
    migrate.migrate();
#endif

    mNotesManager = new NotesManager(this);
    new NotesAgentAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/NotesAgent"), this, QDBusConnection::ExportAdaptors);

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
        dialog->setAttribute(Qt::WA_NativeWindow, true);
        KWindowSystem::setMainWindow(dialog->windowHandle(), windowId);
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
