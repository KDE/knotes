/*
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
    SPDX-FileContributor: Stephen Kelly <stephen@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "noteshared_export.h"

#include <Akonadi/Attribute>

namespace NoteShared
{
class NOTESHARED_EXPORT NoteLockAttribute : public Akonadi::Attribute
{
public:
    NoteLockAttribute();
    ~NoteLockAttribute() override;

    [[nodiscard]] QByteArray type() const override;

    [[nodiscard]] NoteLockAttribute *clone() const override;

    [[nodiscard]] QByteArray serialized() const override;

    void deserialize(const QByteArray &data) override;
};
}
