/*
   Copyright (C) 2013-2018 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "knoteskeydialog.h"

#include <KShortcutsEditor>
#include <QDialog>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

KNotesKeyDialog::KNotesKeyDialog(KActionCollection *globals, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18n("Configure Shortcuts"));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_keyChooser = new KShortcutsEditor(globals, this);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNotesKeyDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNotesKeyDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, m_keyChooser, &KShortcutsEditor::allDefault);

    mainLayout->addWidget(m_keyChooser);
    mainLayout->addWidget(buttonBox);
    readConfig();
}

KNotesKeyDialog::~KNotesKeyDialog()
{
    writeConfig();
}

void KNotesKeyDialog::readConfig()
{
    KConfigGroup grp(KSharedConfig::openConfig(), "KNotesKeyDialog");
    const QSize size = grp.readEntry("Size", QSize(300, 200));
    if (size.isValid()) {
        resize(size);
    }
}

void KNotesKeyDialog::writeConfig()
{
    KConfigGroup grp(KSharedConfig::openConfig(), "KNotesKeyDialog");
    grp.writeEntry("Size", size());
    grp.sync();
}

void KNotesKeyDialog::insert(KActionCollection *actions)
{
    m_keyChooser->addCollection(actions, i18n("Note Actions"));
}

void KNotesKeyDialog::save()
{
    m_keyChooser->save();
}
