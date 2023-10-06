/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <Akonadi/Item>
#include <QListWidget>

namespace NoteShared
{
class NOTESHARED_EXPORT NoteListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit NoteListWidget(QWidget *parent = nullptr);
    ~NoteListWidget() override;

    void setNotes(const Akonadi::Item::List &notes);
    void addNotes(const Akonadi::Item::List &notes);
    void removeNote(const Akonadi::Item &note);

    [[nodiscard]] Akonadi::Item::Id itemId(QListWidgetItem *item) const;

    [[nodiscard]] Akonadi::Item::Id currentItemId() const;

    [[nodiscard]] Akonadi::Item::List selectedNotes() const;

private:
    void createItem(const Akonadi::Item &note);
    enum listViewData { AkonadiId = Qt::UserRole + 1 };

private:
    Akonadi::Item::List mNotes;
};
}
