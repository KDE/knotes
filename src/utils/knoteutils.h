/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "knotes_export.h"
#include <Akonadi/Item>

namespace KNoteUtils
{
KNOTES_EXPORT void setDefaultValue(Akonadi::Item &item);
KNOTES_EXPORT void updateConfiguration();
}
