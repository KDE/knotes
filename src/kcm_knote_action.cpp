/*
   SPDX-FileCopyrightText: 2021 Alexander Lohnau <alexander.lohnau@gmx.de>
   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <KPluginFactory>
#include <config/noteactionconfig.h>

K_PLUGIN_CLASS_WITH_JSON(NoteShared::NoteActionConfig, "kcm_knote_action.json")

#include "kcm_knote_action.moc"
