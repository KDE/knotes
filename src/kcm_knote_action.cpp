/*
   SPDX-FileCopyrightText: 2021 Alexander Lohnau <alexander.lohnau@gmx.de>
   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <KPluginFactory>
#include <config/noteactionconfig.h>

K_PLUGIN_FACTORY_WITH_JSON(NoteActionConfigFactory, "kcm_knote_action.json", registerPlugin<NoteShared::NoteActionConfig>();)

#include "kcm_knote_action.moc"
