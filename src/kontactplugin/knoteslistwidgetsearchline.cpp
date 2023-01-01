/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knoteslistwidgetsearchline.h"

#include "knotesiconview.h"

#include <KLocalizedString>

static QString normalize(QStringView str)
{
    QString out;
    out.reserve(str.size());
    for (const auto c : str) {
        // case folding
        const auto n = c.toCaseFolded();

        // if the character has a canonical decomposition use that and skip the
        // combining diacritic markers following it
        // see https://en.wikipedia.org/wiki/Unicode_equivalence
        // see https://en.wikipedia.org/wiki/Combining_character
        if (n.decompositionTag() == QChar::Canonical) {
            out.push_back(n.decomposition().at(0));
        }
        // handle compatibility compositions such as ligatures
        // see https://en.wikipedia.org/wiki/Unicode_compatibility_characters
        else if (n.decompositionTag() == QChar::Compat && n.isLetter() && n.script() == QChar::Script_Latin) {
            out.append(n.decomposition());
        } else {
            out.push_back(n);
        }
    }
    return out;
}

KNotesListWidgetSearchLine::KNotesListWidgetSearchLine(QWidget *parent)
    : KListWidgetSearchLine(parent)
{
    setPlaceholderText(i18n("Search notes..."));
}

KNotesListWidgetSearchLine::~KNotesListWidgetSearchLine() = default;

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
    const QString strNormalize{normalize(s)};
    if (normalize(iconView->realName()).contains(strNormalize)) {
        return true;
    }
    if (normalize(iconView->description()).contains(strNormalize)) {
        return true;
    }
    return KListWidgetSearchLine::itemMatches(item, s);
}
