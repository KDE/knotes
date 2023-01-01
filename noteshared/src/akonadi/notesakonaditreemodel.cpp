/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "notesakonaditreemodel.h"

#include <Akonadi/ChangeRecorder>

using namespace NoteShared;

NotesAkonadiTreeModel::NotesAkonadiTreeModel(Akonadi::ChangeRecorder *changeRecorder, QObject *parent)
    : Akonadi::EntityTreeModel(changeRecorder, parent)
{
    setItemPopulationStrategy(Akonadi::EntityTreeModel::ImmediatePopulation);
}

NotesAkonadiTreeModel::~NotesAkonadiTreeModel() = default;
