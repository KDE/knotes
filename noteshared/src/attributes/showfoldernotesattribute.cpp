/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "showfoldernotesattribute.h"

#include <QByteArray>
using namespace NoteShared;
ShowFolderNotesAttribute::ShowFolderNotesAttribute()
{
}

ShowFolderNotesAttribute::~ShowFolderNotesAttribute()
{
}

ShowFolderNotesAttribute *ShowFolderNotesAttribute::clone() const
{
    return new ShowFolderNotesAttribute();
}

QByteArray ShowFolderNotesAttribute::type() const
{
    static const QByteArray sType("showfoldernotesattribute");
    return sType;
}

QByteArray ShowFolderNotesAttribute::serialized() const
{
    return QByteArrayLiteral("-");
}

void ShowFolderNotesAttribute::deserialize(const QByteArray &data)
{
    Q_UNUSED(data)
}
