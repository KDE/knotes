/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <AkonadiCore/Item>
#include <QListWidget>

namespace NoteShared
{
class NoteListWidgetPrivate;
class NOTESHARED_EXPORT NoteListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit NoteListWidget(QWidget *parent = nullptr);
    ~NoteListWidget() override;

    void setNotes(const Akonadi::Item::List &notes);
    void addNotes(const Akonadi::Item::List &notes);
    void removeNote(const Akonadi::Item &note);

    Q_REQUIRED_RESULT Akonadi::Item::Id itemId(QListWidgetItem *item) const;

    Q_REQUIRED_RESULT Akonadi::Item::Id currentItemId() const;

    Q_REQUIRED_RESULT Akonadi::Item::List selectedNotes() const;

private:
    void createItem(const Akonadi::Item &note);
    enum listViewData { AkonadiId = Qt::UserRole + 1 };
    NoteListWidgetPrivate *const d;
};
}

