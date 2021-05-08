/*
   SPDX-FileCopyrightText: 2021 Alexander Lohnau <alexander.lohnau@gmx.de>
   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "knoteconfigmodule.h"
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(KNoteEditorConfigFactory, "knote_config_display.json", registerPlugin<KNoteDisplayConfig>();)

#include "kcm_knote_display.moc"
