/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef NOTESAKONADITREEMODEL_H
#define NOTESAKONADITREEMODEL_H

#include <AkonadiCore/EntityTreeModel>
#include "noteshared_export.h"

namespace Akonadi {
class ChangeRecorder;
}

namespace NoteShared {
class NOTESHARED_EXPORT NotesAkonadiTreeModel : public Akonadi::EntityTreeModel
{
    Q_OBJECT
public:
    explicit NotesAkonadiTreeModel(Akonadi::ChangeRecorder *changeRecorder, QObject *parent = nullptr);
    ~NotesAkonadiTreeModel();
};
}

#endif // KNOTESAKONADITREEMODEL_H
