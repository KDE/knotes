/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

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
    Q_REQUIRED_RESULT bool blockSave() const;

protected:
    bool mBlockSave;
};

