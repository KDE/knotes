/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "notelistwidget.h"
#include "attributes/notedisplayattribute.h"
#include "noteutils.h"
#include <KMime/KMimeMessage>

using namespace NoteShared;

NoteListWidget::NoteListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
}

NoteListWidget::~NoteListWidget() = default;

void NoteListWidget::addNotes(const Akonadi::Item::List &notes)
{
    for (const Akonadi::Item &note : notes) {
        if (mNotes.contains(note)) {
            continue;
        }
        createItem(note);
        mNotes.append(note);
    }
}

void NoteListWidget::removeNote(const Akonadi::Item &note)
{
    for (int i = 0; i < count(); ++i) {
        if (item(i)->data(AkonadiId) == note.id()) {
            delete item(i);
            mNotes.removeAll(note);
            break;
        }
    }
}

void NoteListWidget::setNotes(const Akonadi::Item::List &notes)
{
    clear();
    mNotes = notes;
    for (const Akonadi::Item &note : std::as_const(mNotes)) {
        createItem(note);
    }
}

void NoteListWidget::createItem(const Akonadi::Item &note)
{
    auto noteMessage = note.payload<KMime::Message::Ptr>();
    if (!noteMessage) {
        return;
    }
    auto item = new QListWidgetItem(this);

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
    const auto lstItems = selectedItems();
    for (QListWidgetItem *item : lstItems) {
        const Akonadi::Item::Id akonadiId = item->data(AkonadiId).toLongLong();
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

#include "moc_notelistwidget.cpp"
