/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <QObject>

#include <memory>

namespace Akonadi
{
class ChangeRecorder;
}
namespace NoteShared
{
class NotesChangeRecorderPrivate;
class NOTESHARED_EXPORT NotesChangeRecorder : public QObject
{
    Q_OBJECT
public:
    explicit NotesChangeRecorder(QObject *parent = nullptr);
    ~NotesChangeRecorder() override;

    Akonadi::ChangeRecorder *changeRecorder() const;

private:
    Akonadi::ChangeRecorder *const mChangeRecorder;
};
}
