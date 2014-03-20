/*
  Copyright (c) 2013, 2014 Montel Laurent <montel@kde.org>

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
#include "noteshared/attributes/notealarmattribute.h"
#include "noteshared/attributes/notelockattribute.h"


#include <KMime/KMimeMessage>

#include <KLocale>
#include <KLocalizedString>
#include <KGlobal>

KNotePrintObject::KNotePrintObject(const Akonadi::Item &item, QObject *parent)
    : QObject(parent),
      mItem(item)
{
}

KNotePrintObject::~KNotePrintObject()
{

}

QString KNotePrintObject::description() const
{
    KMime::Message::Ptr noteMessage = mItem.payload<KMime::Message::Ptr>();
    if ( noteMessage->contentType()->isHTMLText() ) {
        return noteMessage->mainBodyPart()->decodedText();
    } else {
        return noteMessage->mainBodyPart()->decodedText().replace(QLatin1Char('\n'), QLatin1String("<br>"));
    }
}

QString KNotePrintObject::name() const
{
    KMime::Message::Ptr noteMessage = mItem.payload<KMime::Message::Ptr>();
    return noteMessage->subject(false)->asUnicodeString();
}

QString KNotePrintObject::currentDateTime() const
{
    const QDateTime now = QDateTime::currentDateTime();
    return KGlobal::locale()->formatDateTime( now );
}

bool KNotePrintObject::hasAlarm() const
{
    return mItem.hasAttribute<NoteShared::NoteAlarmAttribute>();
}

QString KNotePrintObject::alarm() const
{
    NoteShared::NoteAlarmAttribute *attr = mItem.attribute<NoteShared::NoteAlarmAttribute>();
    if (attr) {
        return KGlobal::locale()->formatDateTime(attr->dateTime());
    }
    return QString();
}

bool KNotePrintObject::isLock() const
{
    return mItem.hasAttribute<NoteShared::NoteLockAttribute>();
}
