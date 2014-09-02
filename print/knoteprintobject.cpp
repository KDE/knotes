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
#include "noteshared/attributes/notedisplayattribute.h"

#include <KMime/KMimeMessage>

#include <QLocale>

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
    if (noteMessage->contentType()->isHTMLText()) {
        return noteMessage->mainBodyPart()->decodedText();
    } else {
        return noteMessage->mainBodyPart()->decodedText().replace(QLatin1Char('\n'), QLatin1String("<br>"));
    }
}

QString KNotePrintObject::name() const
{
    KMime::Message::Ptr noteMessage = mItem.payload<KMime::Message::Ptr>();
    const KMime::Headers::Subject *const subject = noteMessage ? noteMessage->subject(false) : 0;
    return subject ? subject->asUnicodeString() : QString();
}

QString KNotePrintObject::currentDateTime() const
{
    const QDateTime now = QDateTime::currentDateTime();
    return QLocale().toString((now), QLocale::ShortFormat);
}

bool KNotePrintObject::hasAlarm() const
{
    return mItem.hasAttribute<NoteShared::NoteAlarmAttribute>();
}

QString KNotePrintObject::alarm() const
{
    NoteShared::NoteAlarmAttribute *attr = mItem.attribute<NoteShared::NoteAlarmAttribute>();
    if (attr) {
        return QLocale().toString(attr->dateTime().dateTime(), QLocale::LongFormat);
    }
    return QString();
}

bool KNotePrintObject::isLock() const
{
    return mItem.hasAttribute<NoteShared::NoteLockAttribute>();
}

QString KNotePrintObject::backgroundColorName() const
{
    if (mItem.hasAttribute<NoteShared::NoteDisplayAttribute>()) {
        return mItem.attribute<NoteShared::NoteDisplayAttribute>()->backgroundColor().name();
    }
    return QString();
}
