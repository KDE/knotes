/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "localresourcecreator.h"

#include <AkonadiCore/ServerManager>
#include <AkonadiCore/agentinstancecreatejob.h>
#include <AkonadiCore/agentmanager.h>

#include "maildirsettings.h"

#include <Akonadi/Notes/NoteUtils>

#include "noteshared_debug.h"
#include <AkonadiCore/resourcesynchronizationjob.h>
#include <KLocalizedString>

using namespace NoteShared;

LocalResourceCreator::LocalResourceCreator(QObject *parent)
    : QObject(parent)
{
}

QString LocalResourceCreator::akonadiNotesInstanceName()
{
    return QStringLiteral("akonadi_akonotes_resource");
}

void LocalResourceCreator::createIfMissing()
{
    const Akonadi::AgentInstance::List instances = Akonadi::AgentManager::self()->instances();
    bool found = false;
    for (const Akonadi::AgentInstance &instance : instances) {
        if (instance.type().identifier() == akonadiNotesInstanceName()) {
            found = true;
            break;
        }
    }
    if (found) {
        deleteLater();
        return;
    }
    createInstance();
}

void LocalResourceCreator::createInstance()
{
    Akonadi::AgentType notesType = Akonadi::AgentManager::self()->type(akonadiNotesInstanceName());

    auto job = new Akonadi::AgentInstanceCreateJob(notesType);
    connect(job, &Akonadi::AgentInstanceCreateJob::result, this, &LocalResourceCreator::slotInstanceCreated);

    job->start();
}

void LocalResourceCreator::slotInstanceCreated(KJob *job)
{
    if (job->error()) {
        qCWarning(NOTESHARED_LOG) << job->errorString();
        deleteLater();
        return;
    }

    auto createJob = qobject_cast<Akonadi::AgentInstanceCreateJob *>(job);
    Akonadi::AgentInstance instance = createJob->instance();

    instance.setName(i18nc("Default name for resource holding notes", "Local Notes"));
    const auto service = Akonadi::ServerManager::agentServiceName(Akonadi::ServerManager::Resource, instance.identifier());
    org::kde::Akonadi::Maildir::Settings iface(service, QStringLiteral("/Settings"), QDBusConnection::sessionBus(), this);

    // TODO: Make errors user-visible.
    if (!iface.isValid()) {
        qCWarning(NOTESHARED_LOG) << "Failed to obtain D-Bus interface for remote configuration.";
        deleteLater();
        return;
    }
    instance.reconfigure();

    auto syncJob = new Akonadi::ResourceSynchronizationJob(instance, this);
    connect(syncJob, &Akonadi::ResourceSynchronizationJob::result, this, &LocalResourceCreator::slotSyncDone);
    syncJob->start();
}

void LocalResourceCreator::slotSyncDone(KJob *job)
{
    if (job->error()) {
        qCWarning(NOTESHARED_LOG) << "Synchronizing the resource failed:" << job->errorString();
        deleteLater();
        return;
    }

    qCWarning(NOTESHARED_LOG) << "Instance synchronized";
    deleteLater();
}

void LocalResourceCreator::finishCreateResource()
{
    deleteLater();
}
