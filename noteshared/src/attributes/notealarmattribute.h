/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <Akonadi/Attribute>

#include <QDateTime>
namespace NoteShared
{
class NOTESHARED_EXPORT NoteAlarmAttribute : public Akonadi::Attribute
{
public:
    NoteAlarmAttribute();
    ~NoteAlarmAttribute() override;

    [[nodiscard]] QByteArray type() const override;

    NoteAlarmAttribute *clone() const override;

    [[nodiscard]] QByteArray serialized() const override;

    void deserialize(const QByteArray &data) override;

    void setDateTime(const QDateTime &dateTime);
    [[nodiscard]] QDateTime dateTime() const;

    [[nodiscard]] bool operator==(const NoteAlarmAttribute &other) const;

private:
    QDateTime mDateTime;
};
}
