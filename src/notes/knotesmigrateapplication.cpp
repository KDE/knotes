/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knotesmigrateapplication.h"
#if KCOREADDONS_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <Kdelibs4ConfigMigrator>

KNotesMigrateApplication::KNotesMigrateApplication()
{
    initializeMigrator();
}

void KNotesMigrateApplication::migrate()
{
    // Migrate to xdg.
    Kdelibs4ConfigMigrator migrate(QStringLiteral("knotes"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("knotesrc"));
    migrate.migrate();

    // Migrate folders and files.
    if (mMigrator.checkIfNecessary()) {
        mMigrator.start();
    }
}

void KNotesMigrateApplication::initializeMigrator()
{
    const int currentVersion = 2;
    mMigrator.setApplicationName(QStringLiteral("knotes"));
    mMigrator.setConfigFileName(QStringLiteral("knotesrc"));

    // To migrate we need a version > currentVersion
    const int initialVersion = currentVersion + 1;

    // Templates
    PimCommon::MigrateFileInfo migrateInfoThemePrint;
    migrateInfoThemePrint.setFolder(true);
    migrateInfoThemePrint.setType(QStringLiteral("data"));
    migrateInfoThemePrint.setPath(QStringLiteral("knotes/print/"));
    migrateInfoThemePrint.setVersion(initialVersion);
    mMigrator.insertMigrateInfo(migrateInfoThemePrint);
}
#endif
