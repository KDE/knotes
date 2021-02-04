/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "notealarmattribute.h"

#include <QByteArray>
#include <QDataStream>

using namespace NoteShared;
NoteAlarmAttribute::NoteAlarmAttribute()
    : Akonadi::Attribute()
{
}

NoteAlarmAttribute::~NoteAlarmAttribute()
{
}

NoteAlarmAttribute *NoteAlarmAttribute::clone() const
{
    auto attr = new NoteAlarmAttribute();
    attr->setDateTime(dateTime());
    return attr;
}

void NoteAlarmAttribute::deserialize(const QByteArray &data)
{
    QDataStream s(data);
    s >> mDateTime;
}

QByteArray NoteAlarmAttribute::serialized() const
{
    QByteArray result;
    QDataStream s(&result, QIODevice::WriteOnly);
    s << mDateTime;
    return result;
}

QByteArray NoteAlarmAttribute::type() const
{
    static const QByteArray sType("NoteAlarmAttribute");
    return sType;
}

void NoteAlarmAttribute::setDateTime(const QDateTime &dateTime)
{
    mDateTime = dateTime;
}

QDateTime NoteAlarmAttribute::dateTime() const
{
    return mDateTime;
}

bool NoteAlarmAttribute::operator==(const NoteAlarmAttribute &other) const
{
    return mDateTime == other.dateTime();
}
