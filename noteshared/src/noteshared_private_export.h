/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "noteshared_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
#ifndef NOTESHARED_TESTS_EXPORT
#define NOTESHARED_TESTS_EXPORT NOTESHARED_EXPORT
#endif
#else /* not compiling tests */
#define NOTESHARED_TESTS_EXPORT
#endif

