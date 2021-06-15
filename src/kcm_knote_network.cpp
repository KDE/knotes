/*
   SPDX-FileCopyrightText: 2021 Alexander Lohnau <alexander.lohnau@gmx.de>
   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <KPluginFactory>
#include <config/notenetworkconfig.h>

K_PLUGIN_FACTORY_WITH_JSON(NoteNetworkConfigFactory, "kcm_knote_network.json", registerPlugin<NoteShared::NoteNetworkConfig>();)

#include "kcm_knote_network.moc"
