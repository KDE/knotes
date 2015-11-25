/*
  Copyright (c) 2014-2015 Montel Laurent <montel@kde.org>

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

#include "knotedeleteselectednotesdialog.h"
#include "knotes/noteshared/src/widget/notelistwidget.h"

#include <KLocalizedString>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <KConfigGroup>
#include <QPushButton>
#include <QVBoxLayout>

KNoteDeleteSelectedNotesDialog::KNoteDeleteSelectedNotesDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18n("Select notes to delete"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setDefault(true);
    mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNoteDeleteSelectedNotesDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNoteDeleteSelectedNotesDialog::reject);
    connect(mNoteList, &QListWidget::itemSelectionChanged, this, &KNoteDeleteSelectedNotesDialog::slotDeleteNoteSelectionChanged);

    mNoteList = new NoteShared::NoteListWidget;
    mainLayout->addWidget(mNoteList);
    mainLayout->addWidget(buttonBox);

    readConfig();
    mOkButton->setEnabled(false);
}

KNoteDeleteSelectedNotesDialog::~KNoteDeleteSelectedNotesDialog()
{
    writeConfig();
}

void KNoteDeleteSelectedNotesDialog::slotDeleteNoteSelectionChanged()
{
    mOkButton->setEnabled(!mNoteList->selectedItems().isEmpty());
}

void KNoteDeleteSelectedNotesDialog::setNotes(const Akonadi::Item::List &notes)
{
    mNoteList->addNotes(notes);
}

Akonadi::Item::List KNoteDeleteSelectedNotesDialog::selectedNotes() const
{
    return mNoteList->selectedNotes();
}

void KNoteDeleteSelectedNotesDialog::readConfig()
{
    KConfigGroup grp(KSharedConfig::openConfig(), "KNoteDeleteSelectedNotesDialog");
    const QSize size = grp.readEntry("Size", QSize(300, 200));
    if (size.isValid()) {
        resize(size);
    }
}

void KNoteDeleteSelectedNotesDialog::writeConfig()
{
    KConfigGroup grp(KSharedConfig::openConfig(), "KNoteDeleteSelectedNotesDialog");
    grp.writeEntry("Size", size());
    grp.sync();
}
