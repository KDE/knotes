/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knoteinterface.h"

KNoteInterface::KNoteInterface()
    : mBlockSave(false)
{
}

void KNoteInterface::setBlockSave(bool b)
{
    mBlockSave = b;
}

bool KNoteInterface::blockSave() const
{
    return mBlockSave;
}
