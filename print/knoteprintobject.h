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

#ifndef KNOTEPRINTOBJECT_H
#define KNOTEPRINTOBJECT_H

#include <QObject>
#include <qmetatype.h>
#include "knotes_export.h"

#include <AkonadiCore/Item>

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
    explicit KNotePrintObject(const Akonadi::Item &item, QObject *parent=0);
    ~KNotePrintObject();

    QString description() const;
    QString name() const;
    QString currentDateTime() const;
    bool hasAlarm() const;
    QString alarm() const;
    bool isLock() const;
    QString backgroundColorName() const;
private:
    Akonadi::Item mItem;
};

Q_DECLARE_METATYPE(QList<KNotePrintObject*>)
#endif // KNOTEPRINTOBJECT_H
