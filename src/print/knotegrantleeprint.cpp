/*
   Copyright (C) 2015-2019 Montel Laurent <montel@kde.org>

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
        notes << QVariant::fromValue(static_cast<QObject *>(n));
    }
    QVariantHash variantList;
    variantList.insert(QStringLiteral("notes"), notes);
    variantList.insert(QStringLiteral("alarm_i18n"), i18n("Alarm:"));
    variantList.insert(QStringLiteral("note_is_locked_i18n"), i18n("Note is locked"));

    const QString htmlText = render(variantList);
    return htmlText;
}
