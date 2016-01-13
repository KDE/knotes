/*
  Copyright (c) 2015-2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "knotegrantleeprint.h"
#include "knoteprintobject.h"
#include <KLocalizedString>

KNoteGrantleePrint::KNoteGrantleePrint(QObject *parent)
    : PimCommon::GenericGrantleeFormatter(parent)
{
}

KNoteGrantleePrint::KNoteGrantleePrint(const QString &themePath, QObject *parent)
    : PimCommon::GenericGrantleeFormatter(QStringLiteral("theme.html"), themePath, parent)
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
    Q_FOREACH (KNotePrintObject *n, lst) {
        notes << QVariant::fromValue(static_cast<QObject *>(n));
    }
    QVariantHash variantList;
    variantList.insert(QStringLiteral("notes"), notes);
    variantList.insert(QStringLiteral("alarm_i18n"), i18n("Alarm:"));
    variantList.insert(QStringLiteral("note_is_locked_i18n"), i18n("Note is locked"));

    const QString htmlText = render(variantList);
    return htmlText;
}
