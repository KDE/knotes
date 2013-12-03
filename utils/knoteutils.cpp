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

#include "knoteutils.h"

#include <KStandardDirs>
#include <KMessageBox>
#include <KLocale>
#include <KProcess>
#include <KDebug>
#include <KConfigGroup>

#include <QFileInfo>

void KNoteUtils::migrateToAkonadi()
{
    bool needMigration = true;

    const QFileInfo oldDataDirFileInfo( KStandardDirs::locateLocal( "data", QLatin1String("knotes") ) );
    if ( !oldDataDirFileInfo.exists() || !oldDataDirFileInfo.isDir() ) {
        // neither config or data, the migrator cannot do anything useful anyways
        needMigration = false;
    }

    KConfig config( QLatin1String("knotes-migratorrc") );
    KConfigGroup migrationCfg( &config, "Migration" );
    if ( needMigration ) {
        const bool enabled = migrationCfg.readEntry( "Enabled", true );
        const int currentVersion = migrationCfg.readEntry( "Version", 0 );
        const int targetVersion = migrationCfg.readEntry( "TargetVersion", 1 );
        if ( enabled && currentVersion < targetVersion ) {
            const int choice = KMessageBox::questionYesNoCancel( 0, i18n(
                                                                     "<b>Thanks for using KNotes!</b>"
                                                                     "<p>KNotes uses a new storage technology that requires migration of your current KNotes data and configuration.</p>\n"
                                                                     "<p>The conversion process can take a lot of time (depending on the amount of email you have) and it <em>must not be interrupted</em>.</p>\n"
                                                                     "<p>You can:</p><ul>"
                                                                     "<li>Migrate now (be prepared to wait)</li>"
                                                                     "<li>Skip the migration and start with fresh data and configuration</li>"
                                                                     "<li>Cancel and exit KNotes.</li>"
                                                                     "</ul>"
                                                                     "<p><a href=\"http://userbase.kde.org/Akonadi\">More Information...</a></p>"
                                                                     ), i18n( "KNotes Migration" ), KGuiItem(i18n( "Migrate Now" )), KGuiItem(i18n( "Skip Migration" )), KStandardGuiItem::cancel(),
                                                                 QString(), KMessageBox::Notify | KMessageBox::Dangerous | KMessageBox::AllowLink );
            if ( choice == KMessageBox::Cancel )
                exit( 1 );

            if ( choice != KMessageBox::Yes ) {  // user skipped migration
                // we only will make one attempt at this
                migrationCfg.writeEntry( "Version", targetVersion );
                migrationCfg.sync();

                return;
            }

            kDebug() << "Performing Akonadi migration. Good luck!";
            KProcess proc;
            QStringList args = QStringList() << QLatin1String("--interactive-on-change");
            const QString path = KStandardDirs::findExe( QLatin1String("knotes-migrator" ) );
            proc.setProgram( path, args );
            proc.start();
            bool result = proc.waitForStarted();
            if ( result ) {
                result = proc.waitForFinished( -1 );
            }
            if ( result && proc.exitCode() == 0 ) {
                kDebug() << "Akonadi migration has been successful";
            } else {
                // exit code 1 means it is already running, so we are probably called by a migrator instance
                kError() << "Akonadi migration failed!";
                kError() << "command was: " << proc.program();
                kError() << "exit code: " << proc.exitCode();
                kError() << "stdout: " << proc.readAllStandardOutput();
                kError() << "stderr: " << proc.readAllStandardError();

                KMessageBox::error( 0, i18n("Migration to KNotes failed. In case you want to try again, run 'knotes-migrator --interactive' manually."),
                                    i18n( "Migration Failed" ) );
                return;
            }
        }
    } else {
        if (migrationCfg.hasKey("Enabled") && (migrationCfg.readEntry("Enabled", false) == false)) {
            return;
        }
        migrationCfg.writeEntry( "Enabled", false );
        migrationCfg.sync();
    }
}
