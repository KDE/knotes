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
#include <QDebug>

#include <kcal/journal.h>

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
    return mJournal->description();
}

QString KNotePrintObject::name() const
{
    return mJournal->summary();
}

#include "knoteprintobject.moc"
