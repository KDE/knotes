/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

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

#ifndef KNOTEUTILS_H
#define KNOTEUTILS_H
#include "knotes_export.h"
#include <Akonadi/Item>

namespace KNoteUtils
{
KNOTES_EXPORT void migrateToAkonadi();
KNOTES_EXPORT void setDefaultValue(Akonadi::Item &item);
}

#endif // KNOTEUTILS_H
