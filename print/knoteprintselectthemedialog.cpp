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
#include "knoteprintselectthemecombobox.h"

#include <KLocale>
#include <KGlobal>

#include <QHBoxLayout>
#include <QLabel>

KNotePrintSelectThemeDialog::KNotePrintSelectThemeDialog(QWidget *parent)
    : KDialog(parent)
{
    setCaption( i18n( "Select theme" ) );
    setButtons( User1 | Ok | Cancel );

    QWidget *w = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout;
    w->setLayout(lay);

    QLabel *lab = new QLabel(i18n("Themes:"));
    lay->addWidget(lab);

    mThemes = new KNotePrintSelectThemeComboBox;
    mThemes->loadThemes();
    lay->addWidget(mThemes);
    setMainWidget(w);
}

KNotePrintSelectThemeDialog::~KNotePrintSelectThemeDialog()
{

}

QString KNotePrintSelectThemeDialog::selectedTheme() const
{
    return mThemes->selectedTheme();
}

#include "knoteprintselectthemedialog.moc"
