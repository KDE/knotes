/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "kcmknotessummary.h"

#include <Akonadi/Notes/NoteUtils>

#include <PimCommonAkonadi/CheckedCollectionWidget>

#include <AkonadiWidgets/ETMViewStateSaver>

#include "knotes_kontact_plugin_debug.h"
#include <KAboutData>
#include <KAcceleratorManager>
#include <KCheckableProxyModel>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>

#include <QCheckBox>
#include <QTreeView>
#include <QVBoxLayout>

K_PLUGIN_CLASS_WITH_JSON(KCMKNotesSummary, "kcmknotessummary.json")

KCMKNotesSummary::KCMKNotesSummary(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    initGUI();

    connect(mCheckedCollectionWidget->folderTreeView(), &QAbstractItemView::clicked, this, &KCMKNotesSummary::modified);

    KAcceleratorManager::manage(this);

    load();

    auto about = new KAboutData(QStringLiteral("kcmknotessummary"),
                                i18n("kcmknotessummary"),
                                QString(),
                                i18n("Notes Summary Configuration Dialog"),
                                KAboutLicense::GPL,
                                i18n("Copyright © 2013-2021 Laurent Montel <montel@kde.org>"));
    about->addAuthor(ki18n("Laurent Montel").toString(), QString(), QStringLiteral("montel@kde.org"));
    setAboutData(about);
}

void KCMKNotesSummary::modified()
{
    Q_EMIT changed(true);
}

void KCMKNotesSummary::initGUI()
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    mCheckedCollectionWidget = new PimCommon::CheckedCollectionWidget(Akonadi::NoteUtils::noteMimeType());
    layout->addWidget(mCheckedCollectionWidget);
}

void KCMKNotesSummary::initFolders()
{
    KSharedConfigPtr _config = KSharedConfig::openConfig(QStringLiteral("kcmknotessummaryrc"));

    mModelState = new KViewStateMaintainer<Akonadi::ETMViewStateSaver>(_config->group("CheckState"), this);
    mModelState->setSelectionModel(mCheckedCollectionWidget->selectionModel());
}

void KCMKNotesSummary::loadFolders()
{
    mModelState->restoreState();
}

void KCMKNotesSummary::storeFolders()
{
    KConfig config(QStringLiteral("kcmknotessummaryrc"));
    mModelState->saveState();
    config.sync();
}

void KCMKNotesSummary::load()
{
    initFolders();
    loadFolders();

    Q_EMIT changed(false);
}

void KCMKNotesSummary::save()
{
    storeFolders();

    Q_EMIT changed(false);
}

void KCMKNotesSummary::defaults()
{
    Q_EMIT changed(true);
}

#include "kcmknotessummary.moc"
