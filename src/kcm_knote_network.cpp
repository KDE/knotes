/*
   SPDX-FileCopyrightText: 2021 Alexander Lohnau <alexander.lohnau@gmx.de>
   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "config/notenetworkconfig.h"
#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(NoteShared::NoteNetworkConfig, "kcm_knote_network.json")

#include "kcm_knote_network.moc"
