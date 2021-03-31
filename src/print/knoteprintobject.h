/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "knotes_export.h"

#include <AkonadiCore/Item>

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

    Q_REQUIRED_RESULT QString description() const;
    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT QString currentDateTime() const;
    Q_REQUIRED_RESULT bool hasAlarm() const;
    Q_REQUIRED_RESULT QString alarm() const;
    Q_REQUIRED_RESULT bool isLock() const;
    Q_REQUIRED_RESULT QString backgroundColorName() const;

private:
    Akonadi::Item mItem;
};

Q_DECLARE_METATYPE(QList<KNotePrintObject *>)
