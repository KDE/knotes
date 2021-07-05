/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knoteprintselectthemecombobox.h"
#include "knotesglobalconfig.h"

#include <KConfig>
#include <KConfigGroup>
#include <QDirIterator>
#include <QStandardPaths>

KNotePrintSelectThemeComboBox::KNotePrintSelectThemeComboBox(QWidget *parent)
    : QComboBox(parent)
{
    loadThemes();
}

KNotePrintSelectThemeComboBox::~KNotePrintSelectThemeComboBox()
{
}

void KNotePrintSelectThemeComboBox::loadThemes()
{
    clear();
    const QString defaultTheme = KNotesGlobalConfig::self()->theme();

    const QString relativePath = QStringLiteral("knotes/print/themes/");
    QStringList themesDirectories = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, relativePath, QStandardPaths::LocateDirectory);
    if (themesDirectories.count() < 2) {
        // Make sure to add local directory
        const QString localDirectory = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1Char('/') + relativePath;
        if (!themesDirectories.contains(localDirectory)) {
            themesDirectories.append(localDirectory);
        }
    }

    for (const QString &directory : std::as_const(themesDirectories)) {
        QDirIterator dirIt(directory, QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot);
        QStringList alreadyLoadedThemeName;
        while (dirIt.hasNext()) {
            dirIt.next();
            const QString themeInfoFile = dirIt.filePath() + QLatin1String("/theme.desktop");
            KConfig config(themeInfoFile);
            KConfigGroup group(&config, QStringLiteral("Desktop Entry"));
            QString name = group.readEntry("Name", QString());
            if (name.isEmpty()) {
                continue;
            }
            if (alreadyLoadedThemeName.contains(name)) {
                int i = 2;
                const QString originalName(name);
                while (alreadyLoadedThemeName.contains(name)) {
                    name = originalName + QStringLiteral(" (%1)").arg(i);
                    ++i;
                }
            }
            const QString printThemePath(dirIt.filePath() + QLatin1Char('/'));
            if (!printThemePath.isEmpty()) {
                alreadyLoadedThemeName << name;
                addItem(name, printThemePath);
            }
        }
    }
    model()->sort(0);
    const int index = findData(defaultTheme);
    setCurrentIndex(index == -1 ? 0 : index);
}

QString KNotePrintSelectThemeComboBox::selectedTheme() const
{
    return itemData(currentIndex()).toString();
}

void KNotePrintSelectThemeComboBox::selectDefaultTheme()
{
    const bool bUseDefaults = KNotesGlobalConfig::self()->useDefaults(true);
    const QString defaultTheme = KNotesGlobalConfig::self()->theme();
    const int index = findData(defaultTheme);
    setCurrentIndex(index == -1 ? 0 : index);
    KNotesGlobalConfig::self()->useDefaults(bUseDefaults);
}
