/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <AkonadiAgentBase/agentbase.h>
class NotesManager;
class NotesAgent : public Akonadi::AgentBase, public Akonadi::AgentBase::ObserverV3
{
    Q_OBJECT
public:
    explicit NotesAgent(const QString &id);
    ~NotesAgent() override;

    void showConfigureDialog(qlonglong windowId = 0);

    void setEnableAgent(bool b);
    Q_REQUIRED_RESULT bool enabledAgent() const;

    void reload();

    Q_REQUIRED_RESULT bool receiveNotes() const;
    void setReceiveNotes(bool b);

    void setPort(int value);
    Q_REQUIRED_RESULT int port() const;

    Q_REQUIRED_RESULT int alarmCheckInterval() const;
    void setAlarmCheckInterval(int value);

    void configurationChanged();

public Q_SLOTS:
    void configure(WId windowId) override;

protected:
    void doSetOnline(bool online) override;

private Q_SLOTS:
    void slotStartAgent();

private:
    bool mAgentInitialized = false;
    NotesManager *mNotesManager = nullptr;
};

