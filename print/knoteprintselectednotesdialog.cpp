/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

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
#include "knoteprintselectthemecombobox.h"
#include "knoteprintobject.h"
#include "knote.h"

#include <KLocale>
#include <KConfigGroup>

#include <QListWidget>
#include <QHBoxLayout>
#include <QLabel>

KNotePrintSelectedNotesDialog::KNotePrintSelectedNotesDialog(QWidget *parent)
    : KDialog(parent)
{
    setCaption( i18n( "Select notes" ) );
    setButtons( Ok | Cancel );
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
    lay->addWidget(mTheme);

    setMainWidget(w);
    readConfig();
}

KNotePrintSelectedNotesDialog::~KNotePrintSelectedNotesDialog()
{
    writeConfig();
}

void KNotePrintSelectedNotesDialog::setNotes(const QMap<QString, KNote *> &notes)
{
    mNotes = notes;
    QMapIterator<QString, KNote *> i(notes);
    while (i.hasNext()) {
        i.next();
        QListWidgetItem *item =new QListWidgetItem(mListNotes);
        item->setText(i.value()->name());
        item->setData(JournalId, i.key());
    }
}

QList<KNotePrintObject *> KNotePrintSelectedNotesDialog::selectedNotes() const
{
    QList<KNotePrintObject *> lstPrintObj;
    QList<QListWidgetItem *> lst = mListNotes->selectedItems ();
    Q_FOREACH(QListWidgetItem *item, lst) {
        const QString journalId = item->data(JournalId).toString();
        if (!journalId.isEmpty()) {
            KNotePrintObject *obj = new KNotePrintObject(mNotes.value(journalId)->journal());
            lstPrintObj.append(obj);
        }
    }
    return lstPrintObj;
}

QString KNotePrintSelectedNotesDialog::selectedTheme() const
{
    return mTheme->selectedTheme();
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

#include "knoteprintselectednotesdialog.moc"
