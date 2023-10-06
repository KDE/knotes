/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "knotes_export.h"

#include <Akonadi/Item>

#include <QObject>

class KNOTES_EXPORT KNotePrintObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString currentDateTime READ currentDateTime)
    Q_PROPERTY(bool hasAlarm READ hasAlarm)
    Q_PROPERTY(QString alarm READ alarm)
    Q_PROPERTY(bool isLock READ isLock)
    Q_PROPERTY(QString backgroundColorName READ backgroundColorName)
public:
    explicit KNotePrintObject(const Akonadi::Item &item, QObject *parent = nullptr);
    ~KNotePrintObject() override;

    [[nodiscard]] QString description() const;
    [[nodiscard]] QString name() const;
    [[nodiscard]] QString currentDateTime() const;
    [[nodiscard]] bool hasAlarm() const;
    [[nodiscard]] QString alarm() const;
    [[nodiscard]] bool isLock() const;
    [[nodiscard]] QString backgroundColorName() const;

private:
    Akonadi::Item mItem;
};
