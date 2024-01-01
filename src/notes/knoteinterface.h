/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "knotes_export.h"
#include <QObject>
class KNOTES_EXPORT KNoteInterface
{
public:
    KNoteInterface();
    void setBlockSave(bool b);
    [[nodiscard]] bool blockSave() const;

protected:
    bool mBlockSave = false;
};
