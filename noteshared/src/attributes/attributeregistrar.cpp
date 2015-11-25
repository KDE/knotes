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
