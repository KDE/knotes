/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <Akonadi/Attribute>

namespace NoteShared
{
class NOTESHARED_EXPORT ShowFolderNotesAttribute : public Akonadi::Attribute
{
public:
    ShowFolderNotesAttribute();
    ~ShowFolderNotesAttribute() override;

    ShowFolderNotesAttribute *clone() const override;
    [[nodiscard]] QByteArray type() const override;
    [[nodiscard]] QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;
};
}
