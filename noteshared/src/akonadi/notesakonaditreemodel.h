/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <AkonadiCore/EntityTreeModel>

namespace Akonadi
{
class ChangeRecorder;
}

namespace NoteShared
{
class NOTESHARED_EXPORT NotesAkonadiTreeModel : public Akonadi::EntityTreeModel
{
    Q_OBJECT
public:
    explicit NotesAkonadiTreeModel(Akonadi::ChangeRecorder *changeRecorder, QObject *parent = nullptr);
    ~NotesAkonadiTreeModel() override;
};
}

