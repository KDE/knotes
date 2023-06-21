/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knoteselectednotesdialog.h"
#include "notes/knote.h"

#include <KConfigGroup>
#include <KLocalizedString>

#include <KSharedConfig>
#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>

KNoteSelectedNotesDialog::KNoteSelectedNotesDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Select notes"));
    auto mainLayout = new QVBoxLayout(this);
    mListNotes = new QListWidget(this);
    mListNotes->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(mListNotes, &QListWidget::itemSelectionChanged, this, &KNoteSelectedNotesDialog::slotSelectionChanged);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setDefault(true);
    mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNoteSelectedNotesDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNoteSelectedNotesDialog::reject);

    mainLayout->addWidget(mListNotes);
    mainLayout->addWidget(buttonBox);

    readConfig();
    slotSelectionChanged();
}

KNoteSelectedNotesDialog::~KNoteSelectedNotesDialog()
{
    writeConfig();
}

void KNoteSelectedNotesDialog::slotSelectionChanged()
{
    const bool hasSelection = (!mListNotes->selectedItems().isEmpty());
    mOkButton->setEnabled(hasSelection);
}

void KNoteSelectedNotesDialog::setNotes(const QHash<Akonadi::Item::Id, KNote *> &notes)
{
    mNotes = notes;
    QHashIterator<Akonadi::Item::Id, KNote *> i(notes);
    while (i.hasNext()) {
        i.next();
        auto item = new QListWidgetItem(mListNotes);
        item->setText(i.value()->name());
        item->setToolTip(i.value()->text());
        item->setData(AkonadiId, i.key());
    }
}

QStringList KNoteSelectedNotesDialog::selectedNotes() const
{
    QStringList lst;
    for (QListWidgetItem *item : mListNotes->selectedItems()) {
        Akonadi::Item::Id akonadiId = item->data(AkonadiId).toLongLong();
        if (akonadiId != -1) {
            lst.append(QString::number(akonadiId));
        }
    }
    return lst;
}

namespace
{
static const char myKNoteSelectedNotesDialogName[] = "KNoteSelectedNotesDialog";
}
void KNoteSelectedNotesDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(300, 200));
    KConfigGroup group(KSharedConfig::openStateConfig(), myKNoteSelectedNotesDialogName);
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void KNoteSelectedNotesDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myKNoteSelectedNotesDialogName);
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}

#include "moc_knoteselectednotesdialog.cpp"
