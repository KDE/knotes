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

#include <QListWidget>
#include <QHBoxLayout>
#include <QLabel>

KNotePrintSelectedNotesDialog::KNotePrintSelectedNotesDialog(QWidget *parent)
    : KDialog(parent),
      mPreview(false)
{
    setCaption( i18n( "Select notes" ) );
    setButtons( User1 | Ok | Cancel );
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

    setButtonIcon(User1, KIcon(QLatin1String("document-print-preview")));
    setButtonText(User1, i18n("Preview"));
    setButtonIcon(Ok, KIcon(QLatin1String("document-print")));
    setButtonText(Ok, i18n("Print"));
    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotPreview()));
    connect(mListNotes, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
    setMainWidget(w);
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
    enableButton(User1, hasSelection);
    enableButtonOk(hasSelection);
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
    KConfigGroup grp( KGlobal::config(), "KNotePrintSelectedNotesDialog" );
    const QSize size = grp.readEntry( "Size", QSize(300, 200) );
    if ( size.isValid() ) {
        resize( size );
    }
}

void KNotePrintSelectedNotesDialog::writeConfig()
{
    KConfigGroup grp( KGlobal::config(), "KNotePrintSelectedNotesDialog" );
    grp.writeEntry( "Size", size() );
    grp.sync();
}

void KNotePrintSelectedNotesDialog::slotPreview()
{
    mPreview = true;
    accept();
}

