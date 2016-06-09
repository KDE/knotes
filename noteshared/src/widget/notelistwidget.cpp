/*
   Copyright (C) 2013-2016 Montel Laurent <montel@kde.org>

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
#include "notelistwidget.h"
#include "attributes/notedisplayattribute.h"
#include <KMime/KMimeMessage>
#include "noteutils.h"

using namespace NoteShared;
class NoteShared::NoteListWidgetPrivate
{
public:
    NoteListWidgetPrivate()
    {

    }

    Akonadi::Item::List mNotes;
};

NoteListWidget::NoteListWidget(QWidget *parent)
    : QListWidget(parent),
      d(new NoteShared::NoteListWidgetPrivate)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
}

NoteListWidget::~NoteListWidget()
{
    delete d;
}

void NoteListWidget::addNotes(const Akonadi::Item::List &notes)
{
    Q_FOREACH (const Akonadi::Item &note, notes) {
        if (d->mNotes.contains(note)) {
            continue;
        }
        createItem(note);
        d->mNotes.append(note);
    }
}

void NoteListWidget::removeNote(const Akonadi::Item &note)
{
    for (int i = 0; i < count(); ++i) {
        if (item(i)->data(AkonadiId) == note.id()) {
            delete item(i);
            d->mNotes.removeAll(note);
            break;
        }
    }
}

void NoteListWidget::setNotes(const Akonadi::Item::List &notes)
{
    clear();
    d->mNotes = notes;
    Q_FOREACH (const Akonadi::Item &note, d->mNotes) {
        createItem(note);
    }
}

void NoteListWidget::createItem(const Akonadi::Item &note)
{
    KMime::Message::Ptr noteMessage = note.payload<KMime::Message::Ptr>();
    if (!noteMessage) {
        return;
    }
    QListWidgetItem *item = new QListWidgetItem(this);

    const KMime::Headers::Subject *const subject = noteMessage->subject(false);
    if (subject) {
        item->setText(subject->asUnicodeString());
    }

    NoteShared::NoteUtils noteUtils;
    item->setToolTip(noteUtils.createToolTip(note));
    item->setData(AkonadiId, note.id());
}

Akonadi::Item::List NoteListWidget::selectedNotes() const
{
    Akonadi::Item::List lst;
    Q_FOREACH (QListWidgetItem *item, selectedItems()) {
        Akonadi::Item::Id akonadiId = item->data(AkonadiId).toLongLong();
        if (akonadiId != -1) {
            lst.append(Akonadi::Item(akonadiId));
        }
    }
    return lst;
}

Akonadi::Item::Id NoteListWidget::itemId(QListWidgetItem *item) const
{
    if (item) {
        return item->data(AkonadiId).toLongLong();
    }
    return -1;
}

Akonadi::Item::Id NoteListWidget::currentItemId() const
{
    QListWidgetItem *item = currentItem();
    return itemId(item);
}
