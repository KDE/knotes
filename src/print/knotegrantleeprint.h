/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "knotes_export.h"

#include <GrantleeTheme/GenericFormatter>

class KNotePrintObject;

class KNOTES_EXPORT KNoteGrantleePrint : public GrantleeTheme::GenericFormatter
{
public:
    KNoteGrantleePrint();
    explicit KNoteGrantleePrint(const QString &themePath);
    ~KNoteGrantleePrint();

    Q_REQUIRED_RESULT QString notesToHtml(const QList<KNotePrintObject *> &lst);
};

