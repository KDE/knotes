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

#include "knoteselectednotesdialog.h"
#include "notes/knote.h"

#include <KLocalizedString>
#include <KConfigGroup>

#include <QListWidget>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

KNoteSelectedNotesDialog::KNoteSelectedNotesDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle( i18n( "Select notes" ) );
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setDefault(true);
    mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNoteSelectedNotesDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNoteSelectedNotesDialog::reject);

    mListNotes = new QListWidget;
    mListNotes->setSelectionMode(QAbstractItemView::ExtendedSelection);


    connect(mListNotes, &QListWidget::itemSelectionChanged, this, &KNoteSelectedNotesDialog::slotSelectionChanged);
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
    const bool hasSelection = (mListNotes->selectedItems().count() > 0);
    mOkButton->setEnabled(hasSelection);
}

void KNoteSelectedNotesDialog::setNotes(const QHash<Akonadi::Item::Id, KNote*> &notes)
{
    mNotes = notes;
    QHashIterator<Akonadi::Item::Id, KNote *> i(notes);
    while (i.hasNext()) {
        i.next();
        QListWidgetItem *item =new QListWidgetItem(mListNotes);
        item->setText(i.value()->name());
        item->setToolTip(i.value()->text());
        item->setData(AkonadiId, i.key());
    }
}

QStringList KNoteSelectedNotesDialog::selectedNotes() const
{
    QStringList lst;
    Q_FOREACH(QListWidgetItem *item, mListNotes->selectedItems()) {
        Akonadi::Item::Id akonadiId = item->data(AkonadiId).toLongLong();
        if (akonadiId != -1) {
            lst.append(QString::number(akonadiId));
        }
    }
    return lst;
}

void KNoteSelectedNotesDialog::readConfig()
{
    KConfigGroup grp( KSharedConfig::openConfig(), "KNoteSelectedNotesDialog" );
    const QSize size = grp.readEntry( "Size", QSize(300, 200) );
    if ( size.isValid() ) {
        resize( size );
    }
}

void KNoteSelectedNotesDialog::writeConfig()
{
    KConfigGroup grp( KSharedConfig::openConfig(), "KNoteSelectedNotesDialog" );
    grp.writeEntry( "Size", size() );
    grp.sync();
}
