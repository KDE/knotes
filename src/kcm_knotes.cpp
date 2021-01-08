/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <KCModule>
#include <config/noteactionconfig.h>
#include <config/notenetworkconfig.h>
#include "knoteconfigmodule.h"

extern "C"
{
Q_DECL_EXPORT KCModule *create_knote_config_display(QWidget *parent)
{
    return new KNoteDisplayConfig(parent);
}
}

extern "C"
{
Q_DECL_EXPORT KCModule *create_knote_config_collection(QWidget *parent)
{
    return new KNoteCollectionConfig(parent);
}
}

extern "C"
{
Q_DECL_EXPORT KCModule *create_knote_config_editor(QWidget *parent)
{
    return new KNoteEditorConfig(parent);
}
}

extern "C"
{
Q_DECL_EXPORT KCModule *create_knote_config_action(QWidget *parent)
{
    return new NoteShared::NoteActionConfig(parent);
}
}

extern "C"
{
Q_DECL_EXPORT KCModule *create_knote_config_network(QWidget *parent)
{
    return new NoteShared::NoteNetworkConfig(parent);
}
}

extern "C"
{
Q_DECL_EXPORT KCModule *create_knote_config_print(QWidget *parent)
{
    return new KNotePrintConfig(parent);
}
}

extern "C"
{
Q_DECL_EXPORT KCModule *create_knote_config_misc(QWidget *parent)
{
    return new KNoteMiscConfig(parent);
}
}
