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

#include "knoteskeydialog.h"

#include <KShortcutsEditor>
#include <QDialog>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

KNotesKeyDialog::KNotesKeyDialog( KActionCollection *globals, QWidget *parent )
    : QDialog( parent )
{
    setWindowTitle( i18n( "Configure Shortcuts" ) );
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|QDialogButtonBox::RestoreDefaults);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNotesKeyDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNotesKeyDialog::reject);
    mainLayout->addWidget(buttonBox);

    m_keyChooser = new KShortcutsEditor( globals, this );
    mainLayout->addWidget(m_keyChooser);
    mainLayout->addWidget(buttonBox);
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()),
             m_keyChooser, SLOT(allDefault()) );
    readConfig();
}

KNotesKeyDialog::~KNotesKeyDialog()
{
    writeConfig();
}

void KNotesKeyDialog::readConfig()
{
    KConfigGroup grp( KSharedConfig::openConfig(), "KNotesKeyDialog" );
    const QSize size = grp.readEntry( "Size", QSize(300, 200) );
    if ( size.isValid() ) {
        resize( size );
    }
}

void KNotesKeyDialog::writeConfig()
{
    KConfigGroup grp( KSharedConfig::openConfig(), "KNotesKeyDialog" );
    grp.writeEntry( "Size", size() );
    grp.sync();
}

void KNotesKeyDialog::insert( KActionCollection *actions )
{
    m_keyChooser->addCollection( actions, i18n( "Note Actions" ) );
}

void KNotesKeyDialog::save()
{
    m_keyChooser->save();
}
