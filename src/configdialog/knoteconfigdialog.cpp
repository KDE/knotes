/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2005 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include "knoteconfigdialog.h"
#include "config/noteactionconfig.h"
#include "config/notenetworkconfig.h"
#include "kcoreaddons_version.h"
#include "knotesglobalconfig.h"
#include "notesharedglobalconfig.h"
#include "print/knoteprintselectthemecombobox.h"
#include <config-knotes.h>

#include <KAuthorized>
#include <KNS3/DownloadDialog>
#include <KPluginMetaData>

#include <QCheckBox>
#include <QPushButton>
#include <QWhatsThis>

KNoteConfigDialog::KNoteConfigDialog(const QString &title, QWidget *parent)
    : KCMultiDialog(parent)
{
    setFaceType(KPageDialog::List);
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults);
    button(QDialogButtonBox::Ok)->setDefault(true);

    setWindowTitle(title);
    const QVector<KPluginMetaData> availablePlugins = KPluginMetaData::findPlugins(QStringLiteral("pim/kcms/knotes"));
    for (const KPluginMetaData &metaData : availablePlugins) {
        addModule(metaData);
    }

    connect(button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &KNoteConfigDialog::slotOk);
    connect(button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &KNoteConfigDialog::slotDefaultClicked);
}

KNoteConfigDialog::~KNoteConfigDialog()
{
}

void KNoteConfigDialog::slotOk()
{
    NoteShared::NoteSharedGlobalConfig::self()->save();
    KNotesGlobalConfig::self()->save();
}
