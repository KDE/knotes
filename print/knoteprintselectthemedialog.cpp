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

#include "knoteprintselectthemedialog.h"

#include <KLocale>
#include <KStandardDirs>
#include <KGlobal>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QDir>
#include <QDebug>
#include <QDirIterator>

KNotePrintSelectThemeDialog::KNotePrintSelectThemeDialog(QWidget *parent)
    : KDialog(parent)
{
    setCaption( i18n( "Select theme" ) );
    setButtons( Ok | Cancel );

    QWidget *w = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout;
    w->setLayout(lay);

    QLabel *lab = new QLabel(i18n("Themes:"));
    lay->addWidget(lab);

    mThemes = new QComboBox;
    lay->addWidget(mThemes);
    loadThemes();
    setMainWidget(w);
}

KNotePrintSelectThemeDialog::~KNotePrintSelectThemeDialog()
{

}

void KNotePrintSelectThemeDialog::loadThemes()
{
    const QString relativePath = QLatin1String("knotes/print/themes/");
    QStringList themesDirectories = KGlobal::dirs()->findDirs("data", relativePath);
    if (themesDirectories.count() < 2) {
        //Make sure to add local directory
        const QString localDirectory = KStandardDirs::locateLocal("data", relativePath);
        if (!themesDirectories.contains(localDirectory)) {
            themesDirectories.append(localDirectory);
        }
    }

    Q_FOREACH (const QString &directory, themesDirectories) {
        QDirIterator dirIt( directory, QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot );
        QStringList alreadyLoadedThemeName;
        while ( dirIt.hasNext() ) {
            dirIt.next();
            const QString dirName = dirIt.fileName();
            qDebug()<<" dirName "<<dirName;

            const QString themeInfoFile = dirIt.filePath() + QDir::separator() + QLatin1String("theme.desktop");
            KConfig config( themeInfoFile );
            KConfigGroup group( &config, QLatin1String( "Desktop Entry" ) );
            QString name = group.readEntry( "Name", QString() );
            const QString filename = group.readEntry( "FileName", QString() );
            if (name.isEmpty() || filename.isEmpty()) {
                continue;
            }
            if (alreadyLoadedThemeName.contains(name)) {
                int i = 2;
                const QString originalName(name);
                while (alreadyLoadedThemeName.contains(name)) {
                    name = originalName + QString::fromLatin1(" (%1)").arg(i);
                    ++i;
                }
            }
            const QString printThemePath(dirIt.filePath() + QDir::separator() + filename);
            if (!printThemePath.isEmpty()) {
                alreadyLoadedThemeName << name;
                mThemes->addItem(name, printThemePath);
            }
        }
    }
}

QString KNotePrintSelectThemeDialog::selectedTheme() const
{
    return mThemes->itemData(mThemes->currentIndex()).toString();
}

#include "knoteprintselectthemedialog.moc"
