/*
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
    SPDX-FileContributor: Stephen Kelly <stephen@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "noteshared_export.h"

#include <AkonadiCore/Attribute>

namespace NoteShared
{
class NOTESHARED_EXPORT NoteLockAttribute : public Akonadi::Attribute
{
public:
    NoteLockAttribute();
    ~NoteLockAttribute() override;

    Q_REQUIRED_RESULT QByteArray type() const override;

    Q_REQUIRED_RESULT NoteLockAttribute *clone() const override;

    Q_REQUIRED_RESULT QByteArray serialized() const override;

    void deserialize(const QByteArray &data) override;
};
}

