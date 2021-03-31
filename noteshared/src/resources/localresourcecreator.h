/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <QObject>

class KJob;
namespace NoteShared
{
class NOTESHARED_EXPORT LocalResourceCreator : public QObject
{
    Q_OBJECT
public:
    explicit LocalResourceCreator(QObject *parent = nullptr);

    void createIfMissing();

    static Q_REQUIRED_RESULT QString akonadiNotesInstanceName();

protected:
    virtual void finishCreateResource();

private:
    void createInstance();

private Q_SLOTS:
    void slotInstanceCreated(KJob *job);
    void slotSyncDone(KJob *job);
};
}

