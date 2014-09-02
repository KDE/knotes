/*
  Copyright (c) 2013, 2014 Montel Laurent <montel@kde.org>

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

#include <KLocalizedString>

#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

KNotePrintSelectThemeDialog::KNotePrintSelectThemeDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18n("Select theme"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    QPushButton *user1Button = new QPushButton;
    buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNotePrintSelectThemeDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNotePrintSelectThemeDialog::reject);

    QWidget *w = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout;
    w->setLayout(lay);

    QLabel *lab = new QLabel(i18n("Themes:"));
    lay->addWidget(lab);

    mThemes = new KNotePrintSelectThemeComboBox;
    mThemes->loadThemes();
    lay->addWidget(mThemes);
    mainLayout->addWidget(w);
    mainLayout->addWidget(buttonBox);
}

KNotePrintSelectThemeDialog::~KNotePrintSelectThemeDialog()
{

}

QString KNotePrintSelectThemeDialog::selectedTheme() const
{
    return mThemes->selectedTheme();
}

