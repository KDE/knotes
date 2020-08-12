/*
   SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef KNOTEINTERFACE_H
#define KNOTEINTERFACE_H
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

#endif // KNOTEINTERFACE_H
