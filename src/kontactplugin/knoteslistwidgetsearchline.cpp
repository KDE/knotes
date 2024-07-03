/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knoteslistwidgetsearchline.h"

#include "knotesiconview.h"

#include <KLocalizedString>

#include <TextUtils/ConvertText>

KNotesListWidgetSearchLine::KNotesListWidgetSearchLine(QWidget *parent)
    : KListWidgetSearchLine(parent)
{
    setPlaceholderText(i18nc("@info:placeholder", "Search notes..."));
}

KNotesListWidgetSearchLine::~KNotesListWidgetSearchLine() = default;

void KNotesListWidgetSearchLine::updateClickMessage(const QString &shortcutStr)
{
    setPlaceholderText(i18nc("@info:placeholder", "Search notes...<%1>", shortcutStr));
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
    const QString strTextUtilsNormalize{TextUtils::ConvertText::normalize(s)};
    if (TextUtils::ConvertText::normalize(iconView->realName()).contains(strTextUtilsNormalize)) {
        return true;
    }
    if (TextUtils::ConvertText::normalize(iconView->description()).contains(strTextUtilsNormalize)) {
        return true;
    }
    return KListWidgetSearchLine::itemMatches(item, s);
}

#include "moc_knoteslistwidgetsearchline.cpp"
