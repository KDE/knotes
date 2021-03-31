/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <KLocalizedString>
#include <QCommandLineParser>

static void knotesOptions(QCommandLineParser *parser)
{
    parser->addOption(QCommandLineOption(QStringLiteral("skip-note"), i18n("Suppress creation of a new note on a non-unique instance.")));
}

