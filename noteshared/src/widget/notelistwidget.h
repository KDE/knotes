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
#ifndef NOTELISTWIDGET_H
#define NOTELISTWIDGET_H

#include "noteshared_export.h"
#include <QListWidget>
#include <AkonadiCore/Item>

namespace NoteShared
{
class NoteListWidgetPrivate;
class NOTESHARED_EXPORT NoteListWidget : public QListWidget
{
public:
    explicit NoteListWidget(QWidget *parent = Q_NULLPTR);
    ~NoteListWidget();

    void setNotes(const Akonadi::Item::List &notes);
    void addNotes(const Akonadi::Item::List &notes);
    void removeNote(const Akonadi::Item &note);

    Akonadi::Item::Id itemId(QListWidgetItem *item) const;

    Akonadi::Item::Id currentItemId() const;

    Akonadi::Item::List selectedNotes() const;

private:
    void createItem(const Akonadi::Item &note);
    enum listViewData {
        AkonadiId = Qt::UserRole + 1
    };
    NoteListWidgetPrivate *const d;
};
}

#endif // NOTELISTWIDGET_H
