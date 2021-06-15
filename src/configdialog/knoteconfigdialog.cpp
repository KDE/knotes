/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2005 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include <config-knotes.h>
#include <kcmutils_version.h>

#include "config/noteactionconfig.h"
#include "config/notenetworkconfig.h"
#include "knoteconfigdialog.h"
#include "knotesglobalconfig.h"
#include "notesharedglobalconfig.h"
#include "print/knoteprintselectthemecombobox.h"

#include <KAuthorized>
#include <KLocalizedString>
#include <KNS3/DownloadDialog>
#include <KPluginLoader>
#include <KPluginMetaData>

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWhatsThis>

KNoteConfigDialog::KNoteConfigDialog(const QString &title, QWidget *parent)
    : KCMultiDialog(parent)
{
    setFaceType(KPageDialog::List);
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults);
    button(QDialogButtonBox::Ok)->setDefault(true);

    setWindowTitle(title);
    const QVector<KPluginMetaData> availablePlugins = KPluginLoader::findPlugins(QStringLiteral("pim/kcms/knotes"));
    for (const KPluginMetaData &metaData : availablePlugins) {
#if KCMUTILS_VERSION >= QT_VERSION_CHECK(5, 84, 0)
        addModule(metaData);
#else
        addModule(metaData.pluginId());
#endif
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
