/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2021 Alexander Lohnau <alexander.lohnau@gmx.de>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knoteconfigmodule.h"

#include "config/noteactionconfig.h"
#include "config/notenetworkconfig.h"
#include "knotesglobalconfig.h"
#include "notesharedglobalconfig.h"
#include "print/knoteprintselectthemecombobox.h"
#include <config-knotes.h>

#include "configdialog/knotecollectionconfigwidget.h"
#include "configdialog/knotedisplayconfigwidget.h"
#include "configdialog/knoteeditorconfigwidget.h"
#include <KAuthorized>
#include <KLocalizedString>
#include <KNewStuff3/KNS3/QtQuickDialogWrapper>

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWhatsThis>

KNoteDisplayConfig::KNoteDisplayConfig(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    auto lay = new QVBoxLayout(this);
    QWidget *w = new KNoteDisplayConfigWidget(true);
    lay->addWidget(w);
    lay->addStretch();
    addConfig(KNotesGlobalConfig::self(), w);
    load();
}

void KNoteDisplayConfig::load()
{
    KCModule::load();
}

void KNoteDisplayConfig::save()
{
    KCModule::save();
}

KNoteEditorConfig::KNoteEditorConfig(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    auto lay = new QVBoxLayout(this);
    QWidget *w = new KNoteEditorConfigWidget(this);
    lay->addWidget(w);
    lay->addStretch();
    addConfig(KNotesGlobalConfig::self(), w);
    load();
}

void KNoteEditorConfig::save()
{
    KCModule::save();
}

void KNoteEditorConfig::load()
{
    KCModule::load();
}

KNoteMiscConfig::KNoteMiscConfig(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    auto lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    auto kcfg_SystemTrayShowNotes = new QCheckBox(i18n("Show number of notes in tray icon"), this);

    kcfg_SystemTrayShowNotes->setObjectName(QStringLiteral("kcfg_SystemTrayShowNotes"));
    lay->addWidget(kcfg_SystemTrayShowNotes);

    auto hbox = new QHBoxLayout;
    lay->addLayout(hbox);
    auto label_DefaultTitle = new QLabel(i18n("Default Title:"), this);
    hbox->addWidget(label_DefaultTitle);

    mDefaultTitle = new QLineEdit(this);
    label_DefaultTitle->setBuddy(mDefaultTitle);
    hbox->addWidget(mDefaultTitle);

    auto howItWorks = new QLabel(i18n("<a href=\"whatsthis\">How does this work?</a>"));
    connect(howItWorks, &QLabel::linkActivated, this, &KNoteMiscConfig::slotHelpLinkClicked);
    lay->addWidget(howItWorks);
    addConfig(KNotesGlobalConfig::self(), this);
    howItWorks->setContextMenuPolicy(Qt::NoContextMenu);
    lay->addStretch();
    load();
    connect(mDefaultTitle, &QLineEdit::textChanged, this, &KNoteMiscConfig::markAsChanged);
}

void KNoteMiscConfig::load()
{
    KCModule::load();
    mDefaultTitle->setText(NoteShared::NoteSharedGlobalConfig::self()->defaultTitle());
}

void KNoteMiscConfig::save()
{
    KCModule::save();
    NoteShared::NoteSharedGlobalConfig::self()->setDefaultTitle(mDefaultTitle->text());
    NoteShared::NoteSharedGlobalConfig::self()->save();
}

void KNoteMiscConfig::defaults()
{
    KCModule::defaults();
    const bool bUseDefaults = NoteShared::NoteSharedGlobalConfig::self()->useDefaults(true);
    mDefaultTitle->setText(NoteShared::NoteSharedGlobalConfig::self()->defaultTitle());
    NoteShared::NoteSharedGlobalConfig::self()->useDefaults(bUseDefaults);
}

void KNoteMiscConfig::slotHelpLinkClicked(const QString &)
{
    const QString help = i18n(
        "<qt>"
        "<p>You can customize title note. "
        "You can use:</p>"
        "<ul>"
        "<li>%d current date (short format)</li>"
        "<li>%l current date (long format)</li>"
        "<li>%t current time</li>"
        "</ul>"
        "</qt>");

    QWhatsThis::showText(QCursor::pos(), help);
}

KNotePrintConfig::KNotePrintConfig(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    auto lay = new QVBoxLayout(this);
    auto w = new QWidget(this);
    lay->addWidget(w);
    auto layout = new QGridLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);

    auto label_PrintAction = new QLabel(i18n("Theme:"), this);
    layout->addWidget(label_PrintAction, 0, 0);

    mSelectTheme = new KNotePrintSelectThemeComboBox(this);
    connect(mSelectTheme, qOverload<int>(&QComboBox::activated), this, &KNotePrintConfig::slotThemeChanged);
    label_PrintAction->setBuddy(mSelectTheme);
    layout->addWidget(mSelectTheme, 0, 1);
    if (KAuthorized::authorize(QStringLiteral("ghns"))) {
        auto getNewTheme = new QToolButton;
        getNewTheme->setIcon(QIcon::fromTheme(QStringLiteral("get-hot-new-stuff")));
        getNewTheme->setToolTip(i18n("Download new printing themes"));
        connect(getNewTheme, &QToolButton::clicked, this, &KNotePrintConfig::slotDownloadNewThemes);
        layout->addWidget(getNewTheme, 0, 2);
    }
    lay->addStretch();
    load();
}

void KNotePrintConfig::slotDownloadNewThemes()
{
    if (!KNS3::QtQuickDialogWrapper(QStringLiteral("kwinswitcher.knsrc")).exec().isEmpty()) {
        mSelectTheme->loadThemes();
    }
}

void KNotePrintConfig::slotThemeChanged()
{
    Q_EMIT changed(true);
}

void KNotePrintConfig::save()
{
    KNotesGlobalConfig::self()->setTheme(mSelectTheme->selectedTheme());
}

void KNotePrintConfig::load()
{
    mSelectTheme->loadThemes();
}

void KNotePrintConfig::defaults()
{
    mSelectTheme->selectDefaultTheme();
    Q_EMIT changed(true);
}

KNoteCollectionConfig::KNoteCollectionConfig(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    auto lay = new QHBoxLayout(this);
    mCollectionConfigWidget = new KNoteCollectionConfigWidget;
    lay->addWidget(mCollectionConfigWidget);
    connect(mCollectionConfigWidget, &KNoteCollectionConfigWidget::emitChanged, this, &KNoteCollectionConfig::markAsChanged);
    load();
}

void KNoteCollectionConfig::save()
{
    mCollectionConfigWidget->save();
}

void KNoteCollectionConfig::load()
{
    // Nothing
}
