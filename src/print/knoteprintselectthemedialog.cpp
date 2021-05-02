/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knoteprintselectthemedialog.h"
#include "knoteprintselectthemecombobox.h"

#include <KLocalizedString>

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

KNotePrintSelectThemeDialog::KNotePrintSelectThemeDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Select theme"));
    auto mainLayout = new QVBoxLayout(this);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);

    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    auto user1Button = new QPushButton;
    buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNotePrintSelectThemeDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNotePrintSelectThemeDialog::reject);

    auto w = new QWidget;
    auto lay = new QHBoxLayout;
    w->setLayout(lay);

    auto lab = new QLabel(i18n("Themes:"));
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
