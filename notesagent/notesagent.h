/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <Akonadi/AgentBase>
class NotesManager;
class NotesAgent : public Akonadi::AgentBase, public Akonadi::AgentBase::ObserverV3
{
    Q_OBJECT
public:
    explicit NotesAgent(const QString &id);
    ~NotesAgent() override;

    void showConfigureDialog(qlonglong windowId = 0);

    void setEnableAgent(bool b);
    [[nodiscard]] bool enabledAgent() const;

    void reload();

    [[nodiscard]] bool receiveNotes() const;
    void setReceiveNotes(bool b);

    void setPort(int value);
    [[nodiscard]] int port() const;

    [[nodiscard]] int alarmCheckInterval() const;
    void setAlarmCheckInterval(int value);

    void configurationChanged();

public Q_SLOTS:
    void configure(WId windowId) override;

protected:
    void doSetOnline(bool online) override;

private:
    void slotStartAgent();
    bool mAgentInitialized = false;
    NotesManager *mNotesManager = nullptr;
};
