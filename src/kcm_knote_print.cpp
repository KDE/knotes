/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knoteconfigmodule.h"
#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(KNotePrintConfig, "knote_config_print.json")

#include "kcm_knote_print.moc"
