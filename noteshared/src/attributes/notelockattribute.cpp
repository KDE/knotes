/*
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
    SPDX-FileContributor: Stephen Kelly <stephen@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "notelockattribute.h"

#include <QByteArray>
using namespace NoteShared;
NoteLockAttribute::NoteLockAttribute()
    : Akonadi::Attribute()
{
}

NoteLockAttribute::~NoteLockAttribute()
{
}

NoteLockAttribute *NoteLockAttribute::clone() const
{
    return new NoteLockAttribute();
}

void NoteLockAttribute::deserialize(const QByteArray &data)
{
    Q_UNUSED(data)
}

QByteArray NoteLockAttribute::serialized() const
{
    return QByteArrayLiteral("-");
}

QByteArray NoteLockAttribute::type() const
{
    // We can't change this name!
    static const QByteArray sType("KJotsLockAttribute");
    return sType;
}
