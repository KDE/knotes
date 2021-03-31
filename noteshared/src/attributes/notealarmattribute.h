/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <AkonadiCore/Attribute>

#include <QDateTime>
namespace NoteShared
{
class NOTESHARED_EXPORT NoteAlarmAttribute : public Akonadi::Attribute
{
public:
    NoteAlarmAttribute();
    ~NoteAlarmAttribute() override;

    Q_REQUIRED_RESULT QByteArray type() const override;

    NoteAlarmAttribute *clone() const override;

    Q_REQUIRED_RESULT QByteArray serialized() const override;

    void deserialize(const QByteArray &data) override;

    void setDateTime(const QDateTime &dateTime);
    Q_REQUIRED_RESULT QDateTime dateTime() const;

    Q_REQUIRED_RESULT bool operator==(const NoteAlarmAttribute &other) const;

private:
    QDateTime mDateTime;
};
}

