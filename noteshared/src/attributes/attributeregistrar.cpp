/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "notealarmattribute.h"
#include "notedisplayattribute.h"
#include "notelockattribute.h"
#include "showfoldernotesattribute.h"

#include <AkonadiCore/attributefactory.h>

namespace
{
// Anonymous namespace; function is invisible outside this file.
bool dummy()
{
    Akonadi::AttributeFactory::registerAttribute<NoteShared::NoteDisplayAttribute>();
    Akonadi::AttributeFactory::registerAttribute<NoteShared::NoteAlarmAttribute>();
    Akonadi::AttributeFactory::registerAttribute<NoteShared::NoteLockAttribute>();
    Akonadi::AttributeFactory::registerAttribute<NoteShared::ShowFolderNotesAttribute>();
    return true;
}

// Called when this library is loaded.
const bool registered = dummy();
} // namespace
