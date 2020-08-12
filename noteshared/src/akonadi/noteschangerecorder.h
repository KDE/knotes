/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef NOTESCHANGERECORDER_H
#define NOTESCHANGERECORDER_H

#include <QObject>
#include "noteshared_export.h"
namespace Akonadi {
class ChangeRecorder;
}
namespace NoteShared {
class NotesChangeRecorderPrivate;
class NOTESHARED_EXPORT NotesChangeRecorder : public QObject
{
    Q_OBJECT
public:
    explicit NotesChangeRecorder(QObject *parent = nullptr);
    ~NotesChangeRecorder();

    Akonadi::ChangeRecorder *changeRecorder() const;

private:
    NotesChangeRecorderPrivate *const d;
};
}

#endif // KNOTESCHANGERECORDER_H
