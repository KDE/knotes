/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2005 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include <config-knotes.h>

#include "knoteconfigdialog.h"
#include "notesharedglobalconfig.h"
#include "knotedisplayconfigwidget.h"
#include "knoteeditorconfigwidget.h"
#include "knotesglobalconfig.h"
#include "config/noteactionconfig.h"
#include "config/notenetworkconfig.h"
#include "print/knoteprintselectthemecombobox.h"

#include <KAuthorized>
#include <KLocalizedString>
#include <KNS3/DownloadDialog>

#include <QCheckBox>
#include <QApplication>
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
    addModule(QStringLiteral("knote_config_display"));
    addModule(QStringLiteral("knote_config_editor"));
    addModule(QStringLiteral("knote_config_action"));
    addModule(QStringLiteral("knote_config_network"));
    addModule(QStringLiteral("knote_config_print"));
    addModule(QStringLiteral("knote_config_collection"));
    addModule(QStringLiteral("knote_config_misc"));

    connect(button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &KNoteConfigDialog::slotOk);

    connect(button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked,
            this, &KNoteConfigDialog::slotDefaultClicked);
}

KNoteConfigDialog::~KNoteConfigDialog()
{
}

void KNoteConfigDialog::slotOk()
{
    NoteShared::NoteSharedGlobalConfig::self()->save();
    KNotesGlobalConfig::self()->save();
}

