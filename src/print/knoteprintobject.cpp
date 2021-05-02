/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knoteprintobject.h"
#include "attributes/notealarmattribute.h"
#include "attributes/notedisplayattribute.h"
#include "attributes/notelockattribute.h"

#include <KMime/KMimeMessage>

#include <QLocale>

KNotePrintObject::KNotePrintObject(const Akonadi::Item &item, QObject *parent)
    : QObject(parent)
    , mItem(item)
{
}

KNotePrintObject::~KNotePrintObject()
{
}

QString KNotePrintObject::description() const
{
    auto noteMessage = mItem.payload<KMime::Message::Ptr>();
    if (noteMessage->contentType()->isHTMLText()) {
        return noteMessage->mainBodyPart()->decodedText();
    } else {
        return noteMessage->mainBodyPart()->decodedText().replace(QLatin1Char('\n'), QStringLiteral("<br>"));
    }
}

QString KNotePrintObject::name() const
{
    auto noteMessage = mItem.payload<KMime::Message::Ptr>();
    const KMime::Headers::Subject *const subject = noteMessage ? noteMessage->subject(false) : nullptr;
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
    const auto attr = mItem.attribute<NoteShared::NoteAlarmAttribute>();
    if (attr) {
        return QLocale().toString(attr->dateTime(), QLocale::LongFormat);
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
