/*
  Copyright (c) 2015-2016 Montel Laurent <montel@kde.org>

  based on code from Sune Vuorela <sune@vuorela.dk> (Rawatar source code)

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

#include "knotesmigrateapplication.h"

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

