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

#include "knoteprintselectednotesdialog.h"
#include "knotes/print/knoteprintselectthemecombobox.h"
#include "knoteprintobject.h"
#include "notes/knote.h"

#include <KLocalizedString>
#include <KConfigGroup>
#include <QIcon>

#include <QListWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

KNotePrintSelectedNotesDialog::KNotePrintSelectedNotesDialog(QWidget *parent)
    : QDialog(parent),
      mPreview(false)
{
    setWindowTitle( i18n( "Select notes" ) );
    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    QPushButton *okButton = mButtonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    mUser1Button = new QPushButton;
    mButtonBox->addButton(mUser1Button, QDialogButtonBox::ActionRole);
    connect(mButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(mButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QWidget *w = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    w->setLayout(vbox);

    mListNotes = new QListWidget;
    mListNotes->setSelectionMode(QAbstractItemView::ExtendedSelection);
    vbox->addWidget(mListNotes);

    QHBoxLayout *lay = new QHBoxLayout;
    lay->setMargin(0);
    vbox->addLayout(lay);
    QLabel *lab = new QLabel(i18n("Printing theme:"));
    lay->addWidget(lab);
    mTheme = new KNotePrintSelectThemeComboBox;
    mTheme->loadThemes();
    lay->addWidget(mTheme);

    mUser1Button->setIcon(QIcon::fromTheme(QLatin1String("document-print-preview")));
    mUser1Button->setText(i18n("Preview"));
    okButton->setIcon(QIcon::fromTheme(QLatin1String("document-print")));
    okButton->setText(i18n("Print"));
    connect(mUser1Button, SIGNAL(clicked()), this, SLOT(slotPreview()));
    connect(mListNotes, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
    mainLayout->addWidget(w);
    mainLayout->addWidget(mButtonBox);
    readConfig();
    slotSelectionChanged();
}

KNotePrintSelectedNotesDialog::~KNotePrintSelectedNotesDialog()
{
    writeConfig();
}

void KNotePrintSelectedNotesDialog::slotSelectionChanged()
{
    const bool hasSelection = (mListNotes->selectedItems().count() > 0);
    mUser1Button->setEnabled(hasSelection);
    mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(hasSelection);
}

void KNotePrintSelectedNotesDialog::setNotes(const QHash<Akonadi::Item::Id, KNote*> &notes)
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

QList<KNotePrintObject *> KNotePrintSelectedNotesDialog::selectedNotes() const
{
    QList<KNotePrintObject *> lstPrintObj;
    QList<QListWidgetItem *> lst = mListNotes->selectedItems ();
    Q_FOREACH(QListWidgetItem *item, lst) {
        Akonadi::Item::Id akonadiId = item->data(AkonadiId).toLongLong();
        if (akonadiId != -1) {
            KNotePrintObject *obj = new KNotePrintObject(mNotes.value(akonadiId)->item());
            lstPrintObj.append(obj);
        }
    }
    return lstPrintObj;
}

QString KNotePrintSelectedNotesDialog::selectedTheme() const
{
    return mTheme->selectedTheme();
}

bool KNotePrintSelectedNotesDialog::preview() const
{
    return mPreview;
}

void KNotePrintSelectedNotesDialog::readConfig()
{
    KConfigGroup grp( KSharedConfig::openConfig(), "KNotePrintSelectedNotesDialog" );
    const QSize size = grp.readEntry( "Size", QSize(300, 200) );
    if ( size.isValid() ) {
        resize( size );
    }
}

void KNotePrintSelectedNotesDialog::writeConfig()
{
    KConfigGroup grp( KSharedConfig::openConfig(), "KNotePrintSelectedNotesDialog" );
    grp.writeEntry( "Size", size() );
    grp.sync();
}

void KNotePrintSelectedNotesDialog::slotPreview()
{
    mPreview = true;
    accept();
}

