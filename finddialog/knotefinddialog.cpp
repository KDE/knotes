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

#include "knotefinddialog.h"
#include "baloo/pim/notequery.h"
#include <baloo/pim/resultiterator.h>

#include <KLocalizedString>
#include <KLineEdit>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>

KNoteFindDialog::KNoteFindDialog(QWidget *parent)
    : KDialog(parent)
{
    setCaption(i18n("Search Notes"));
    setButtons(Close);
    mNoteFindWidget = new KNoteFindWidget;
    setMainWidget(mNoteFindWidget);
    readConfig();
}

KNoteFindDialog::~KNoteFindDialog()
{
    writeConfig();
}

void KNoteFindDialog::writeConfig()
{
    KConfigGroup grp( KGlobal::config(), "KNoteFindDialog" );
    grp.writeEntry( "Size", size() );
    grp.sync();
}

void KNoteFindDialog::readConfig()
{
    KConfigGroup grp( KGlobal::config(), "KNoteFindDialog" );
    const QSize size = grp.readEntry( "Size", QSize(600, 300) );
    if ( size.isValid() ) {
        resize( size );
    }
}


KNoteFindWidget::KNoteFindWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    vbox->addLayout(hbox);
    QLabel *lab = new QLabel(i18n("Search notes:"));
    hbox->addWidget(lab);
    mSearchLineEdit = new KLineEdit;
    mSearchLineEdit->setTrapReturnKey(true);
    mSearchLineEdit->setClearButtonShown(true);
    connect(mSearchLineEdit, SIGNAL(returnPressed()), this, SLOT(slotSearchNote()));
    connect(mSearchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
    hbox->addWidget(mSearchLineEdit);

    mSearchButton = new QPushButton(i18n("Search..."));
    connect(mSearchButton, SIGNAL(clicked(bool)), this, SLOT(slotSearchNote()));
    hbox->addWidget(mSearchButton);
    mSearchButton->setEnabled(false);

    //Result
    mNoteList = new QListWidget;
    connect(mNoteList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotItemDoubleClicked(QListWidgetItem*)));
    vbox->addWidget(mNoteList);
    mSearchLineEdit->setFocus();

    setLayout(vbox);
}

KNoteFindWidget::~KNoteFindWidget()
{

}

void KNoteFindWidget::slotItemDoubleClicked(QListWidgetItem*)
{
    //TODO
}

void KNoteFindWidget::slotSearchNote()
{
    const QString searchStr = mSearchLineEdit->text().trimmed();
    if (searchStr.trimmed().isEmpty())
        return;
    Baloo::PIM::NoteQuery query;
    query.matchNote(searchStr);
    query.matchTitle(searchStr);

    Baloo::PIM::ResultIterator it = query.exec();

    QSet<qint64> ids;
    while (it.next())
        ids << it.id();

    qDebug()<<" QSet<qint64> mMatchingItemIds;"<<ids.count();
}

void KNoteFindWidget::slotTextChanged(const QString &text)
{
    mSearchButton->setEnabled(!text.trimmed().isEmpty());
}
