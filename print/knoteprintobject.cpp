/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

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

#include "knoteprintobject.h"

#include <QTextDocument>

#include <kcal/journal.h>

#include <KLocale>
#include <KGlobal>
#include <KDateTime>

KNotePrintObject::KNotePrintObject(KCal::Journal *journal, QObject *parent)
    : QObject(parent),
      mJournal(journal)
{
}

KNotePrintObject::~KNotePrintObject()
{

}

QString KNotePrintObject::description() const
{
    if ( Qt::mightBeRichText( mJournal->description() ) ) {
        return mJournal->description();
    } else {
        return mJournal->description().replace(QLatin1Char('\n'), QLatin1String("<br>"));
    }
}

QString KNotePrintObject::name() const
{
    return mJournal->summary();
}

QString KNotePrintObject::currentDateTime() const
{
    const QDateTime now = QDateTime::currentDateTime();
    return KGlobal::locale()->formatDateTime( now );
}

#include "knoteprintobject.moc"
