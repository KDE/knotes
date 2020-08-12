/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef KNOTESMIGRATEAPPLICATION_H
#define KNOTESMIGRATEAPPLICATION_H

#include "knotes_export.h"

#include <PimCommon/MigrateApplicationFiles>

class KNOTES_EXPORT KNotesMigrateApplication
{
public:
    KNotesMigrateApplication();
    void migrate();
private:
    void initializeMigrator();
    PimCommon::MigrateApplicationFiles mMigrator;
};

#endif // KNOTESMIGRATEAPPLICATION_H
