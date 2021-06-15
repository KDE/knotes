/*
   SPDX-FileCopyrightText: 2021 Alexander Lohnau <alexander.lohnau@gmx.de>
   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "knoteconfigmodule.h"
#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(KNoteMiscConfig, "kcm_knote_misc.json")

#include "kcm_knote_misc.moc"
