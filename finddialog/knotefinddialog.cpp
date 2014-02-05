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

#include <KLocalizedString>
#include <KLineEdit>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

KNoteFindDialog::KNoteFindDialog(QWidget *parent)
    : KDialog(parent)
{
    setCaption(i18n("Search Notes"));
    setButtons(Close);
    mNoteFindWidget = new KNoteFindWidget;
    setMainWidget(mNoteFindWidget);
}

KNoteFindDialog::~KNoteFindDialog()
{

}


KNoteFindWidget::KNoteFindWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout;
    QLabel *lab = new QLabel(i18n("Search notes:"));
    vbox->addWidget(lab);
    mSearchLineEdit = new KLineEdit;
    connect(mSearchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
    vbox->addWidget(mSearchLineEdit);

    //Result
    mNoteList = new QListWidget;
    vbox->addWidget(mNoteList);


    mSearchButton = new QPushButton(i18n("Search..."));
    connect(mSearchButton, SIGNAL(clicked(bool)), this, SLOT(slotSearchNote()));
    vbox->addWidget(mSearchButton);
    mSearchButton->setEnabled(false);
    setLayout(vbox);
}

KNoteFindWidget::~KNoteFindWidget()
{

}

void KNoteFindWidget::slotSearchNote()
{
    const QString searchStr = mSearchLineEdit->text().trimmed();
    if (searchStr.isEmpty())
        return;
    //TODO
}

void KNoteFindWidget::slotTextChanged(const QString &text)
{
    mSearchButton->setEnabled(!text.isEmpty());
}
