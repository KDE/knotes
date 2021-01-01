/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knotegrantleeprint.h"
#include "knoteprintobject.h"
#include <KLocalizedString>

KNoteGrantleePrint::KNoteGrantleePrint() = default;

KNoteGrantleePrint::KNoteGrantleePrint(const QString &themePath)
    : GrantleeTheme::GenericFormatter(QStringLiteral("theme.html"), themePath)
{
}

KNoteGrantleePrint::~KNoteGrantleePrint()
{
}

QString KNoteGrantleePrint::notesToHtml(const QList<KNotePrintObject *> &lst)
{
    if (lst.isEmpty()) {
        return QString();
    }
    QVariantList notes;
    notes.reserve(lst.count());
    for (KNotePrintObject *n : lst) {
        notes << QVariant::fromValue(n);
    }
    QVariantHash variantList;
    variantList.insert(QStringLiteral("notes"), notes);
    variantList.insert(QStringLiteral("alarm_i18n"), i18n("Alarm:"));
    variantList.insert(QStringLiteral("note_is_locked_i18n"), i18n("Note is locked"));

    const QString htmlText = render(variantList);
    return htmlText;
}
