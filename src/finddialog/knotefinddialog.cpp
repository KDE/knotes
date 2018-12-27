/*
   Copyright (C) 2013-2019 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "knotefinddialog.h"
#include "widget/notelistwidget.h"

#include <AkonadiSearch/PIM/notequery.h>
#include <AkonadiSearch/PIM/resultiterator.h>

#include <KLocalizedString>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>

#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <KConfigGroup>

KNoteFindDialog::KNoteFindDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Search Notes"));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNoteFindDialog::reject);
    setAttribute(Qt::WA_DeleteOnClose);
    mNoteFindWidget = new KNoteFindWidget(this);
    connect(mNoteFindWidget, &KNoteFindWidget::noteSelected, this, &KNoteFindDialog::noteSelected);
    mainLayout->addWidget(mNoteFindWidget);
    mainLayout->addWidget(buttonBox);
    readConfig();
}

KNoteFindDialog::~KNoteFindDialog()
{
    writeConfig();
}

void KNoteFindDialog::setExistingNotes(const QHash<Akonadi::Item::Id, Akonadi::Item> &notes)
{
    mNoteFindWidget->setExistingNotes(notes);
}

void KNoteFindDialog::writeConfig()
{
    KConfigGroup grp(KSharedConfig::openConfig(), "KNoteFindDialog");
    grp.writeEntry("Size", size());
    grp.sync();
}

void KNoteFindDialog::readConfig()
{
    KConfigGroup grp(KSharedConfig::openConfig(), "KNoteFindDialog");
    const QSize size = grp.readEntry("Size", QSize(600, 300));
    if (size.isValid()) {
        resize(size);
    }
}

KNoteFindWidget::KNoteFindWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    QHBoxLayout *hbox = new QHBoxLayout;
    vbox->addLayout(hbox);
    QLabel *lab = new QLabel(i18nc("@label:textbox", "Search notes:"), this);
    hbox->addWidget(lab);
    mSearchLineEdit = new QLineEdit(this);
    mSearchLineEdit->setClearButtonEnabled(true);
    connect(mSearchLineEdit, &QLineEdit::returnPressed, this, &KNoteFindWidget::slotSearchNote);
    connect(mSearchLineEdit, &QLineEdit::textChanged, this, &KNoteFindWidget::slotTextChanged);
    hbox->addWidget(mSearchLineEdit);

    mSearchButton = new QPushButton(QIcon::fromTheme(QStringLiteral("edit-find")),
                                    i18nc("@action:button Search notes", "Search..."),
                                    this);
    connect(mSearchButton, &QPushButton::clicked, this, &KNoteFindWidget::slotSearchNote);
    hbox->addWidget(mSearchButton);
    mSearchButton->setEnabled(false);

    //Result
    mNoteList = new NoteShared::NoteListWidget(this);
    mNoteList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(mNoteList, &NoteShared::NoteListWidget::itemDoubleClicked,
            this, &KNoteFindWidget::slotItemDoubleClicked);
    vbox->addWidget(mNoteList);

    mResultSearch = new QLabel(this);
    mResultSearch->setTextFormat(Qt::PlainText);
    vbox->addWidget(mResultSearch);

    mSearchLineEdit->setFocus();
}

KNoteFindWidget::~KNoteFindWidget()
{
}

void KNoteFindWidget::setExistingNotes(const QHash<Akonadi::Item::Id, Akonadi::Item> &notes)
{
    mNotes = notes;
}

void KNoteFindWidget::slotItemDoubleClicked(QListWidgetItem *item)
{
    Q_EMIT noteSelected(mNoteList->itemId(item));
}

void KNoteFindWidget::slotSearchNote()
{
    const QString searchStr = mSearchLineEdit->text().trimmed();
    if (searchStr.trimmed().isEmpty()) {
        return;
    }
    Akonadi::Search::PIM::NoteQuery query;
    query.matchNote(searchStr);
    query.matchTitle(searchStr);

    Akonadi::Search::PIM::ResultIterator it = query.exec();

    Akonadi::Item::List lst;
    while (it.next()) {
        const Akonadi::Item::Id id = it.id();
        if (mNotes.contains(id)) {
            lst << mNotes.value(id);
        }
    }
    mNoteList->setNotes(lst);
    if (lst.isEmpty()) {
        mResultSearch->setText(i18nc("@label", "No Results found in search."));
    } else {
        mResultSearch->clear();
    }
}

void KNoteFindWidget::slotTextChanged(const QString &text)
{
    mSearchButton->setEnabled(!text.trimmed().isEmpty());
}
