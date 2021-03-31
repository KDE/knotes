/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <AkonadiCore/attribute.h>

namespace NoteShared
{
class NOTESHARED_EXPORT ShowFolderNotesAttribute : public Akonadi::Attribute
{
public:
    ShowFolderNotesAttribute();
    ~ShowFolderNotesAttribute() override;

    ShowFolderNotesAttribute *clone() const override;
    Q_REQUIRED_RESULT QByteArray type() const override;
    Q_REQUIRED_RESULT QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;
};
}

