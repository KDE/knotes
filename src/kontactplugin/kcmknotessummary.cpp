/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "kcmknotessummary.h"

#include <Akonadi/NoteUtils>

#include <PimCommonAkonadi/CheckedCollectionWidget>

#include <Akonadi/ETMViewStateSaver>

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

KCMKNotesSummary::KCMKNotesSummary(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    initGUI();

    connect(mCheckedCollectionWidget->folderTreeView(), &QAbstractItemView::clicked, this, &KCMKNotesSummary::modified);

    KAcceleratorManager::manage(widget());

    load();
}

void KCMKNotesSummary::modified()
{
    markAsChanged();
}

void KCMKNotesSummary::initGUI()
{
    auto layout = new QVBoxLayout(widget());
    layout->setContentsMargins(0, 0, 0, 0);

    mCheckedCollectionWidget = new PimCommon::CheckedCollectionWidget(Akonadi::NoteUtils::noteMimeType());
    layout->addWidget(mCheckedCollectionWidget);
}

void KCMKNotesSummary::initFolders()
{
    KSharedConfigPtr _config = KSharedConfig::openConfig(QStringLiteral("kcmknotessummaryrc"));

    mModelState = new KViewStateMaintainer<Akonadi::ETMViewStateSaver>(_config->group(QStringLiteral("CheckState")), widget());
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

    setNeedsSave(false);
}

void KCMKNotesSummary::save()
{
    storeFolders();
    setNeedsSave(false);
}

void KCMKNotesSummary::defaults()
{
    markAsChanged();
}

#include "kcmknotessummary.moc"

#include "moc_kcmknotessummary.cpp"
