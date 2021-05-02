/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knotedeleteselectednotesdialog.h"
#include "widget/notelistwidget.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

KNoteDeleteSelectedNotesDialog::KNoteDeleteSelectedNotesDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Select notes to delete"));
    auto mainLayout = new QVBoxLayout(this);

    mNoteList = new NoteShared::NoteListWidget(this);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setDefault(true);
    mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNoteDeleteSelectedNotesDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNoteDeleteSelectedNotesDialog::reject);

    mainLayout->addWidget(mNoteList);
    mainLayout->addWidget(buttonBox);

    readConfig();
    mOkButton->setEnabled(false);
    connect(mNoteList, &QListWidget::itemSelectionChanged, this, &KNoteDeleteSelectedNotesDialog::slotDeleteNoteSelectionChanged);
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
    KConfigGroup grp(KSharedConfig::openStateConfig(), "KNoteDeleteSelectedNotesDialog");
    const QSize size = grp.readEntry("Size", QSize(300, 200));
    if (size.isValid()) {
        resize(size);
    }
}

void KNoteDeleteSelectedNotesDialog::writeConfig()
{
    KConfigGroup grp(KSharedConfig::openStateConfig(), "KNoteDeleteSelectedNotesDialog");
    grp.writeEntry("Size", size());
    grp.sync();
}
