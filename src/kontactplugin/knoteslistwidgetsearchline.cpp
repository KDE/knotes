/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knoteslistwidgetsearchline.h"

#include "knotesiconview.h"

#include <KLocalizedString>

KNotesListWidgetSearchLine::KNotesListWidgetSearchLine(QWidget *parent)
    : KListWidgetSearchLine(parent)
{
    setPlaceholderText(i18n("Search notes..."));
}

KNotesListWidgetSearchLine::~KNotesListWidgetSearchLine()
{
}

void KNotesListWidgetSearchLine::updateClickMessage(const QString &shortcutStr)
{
    setPlaceholderText(i18n("Search notes...<%1>", shortcutStr));
}

bool KNotesListWidgetSearchLine::itemMatches(const QListWidgetItem *item, const QString &s) const
{
    if (!item) {
        return false;
    }
    const auto iconView = dynamic_cast<const KNotesIconViewItem *>(item);
    if (!iconView) {
        return false;
    }
    if (iconView->realName().contains(s)) {
        return true;
    }
    if (iconView->description().contains(s)) {
        return true;
    }
    return KListWidgetSearchLine::itemMatches(item, s);
}
