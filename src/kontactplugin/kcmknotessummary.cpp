/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

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

#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
KCMKNotesSummary::KCMKNotesSummary(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
#else
KCMKNotesSummary::KCMKNotesSummary(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : KCModule(parent, data, args)
#endif
{
    initGUI();

    connect(mCheckedCollectionWidget->folderTreeView(), &QAbstractItemView::clicked, this, &KCMKNotesSummary::modified);

#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    KAcceleratorManager::manage(this);
#else
    KAcceleratorManager::manage(widget());
#endif

    load();
}

void KCMKNotesSummary::modified()
{
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    Q_EMIT changed(true);
#else
    markAsChanged();
#endif
}

void KCMKNotesSummary::initGUI()
{
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    auto layout = new QVBoxLayout(this);
#else
    auto layout = new QVBoxLayout(widget());
#endif
    layout->setContentsMargins(0, 0, 0, 0);

    mCheckedCollectionWidget = new PimCommon::CheckedCollectionWidget(Akonadi::NoteUtils::noteMimeType());
    layout->addWidget(mCheckedCollectionWidget);
}

void KCMKNotesSummary::initFolders()
{
    KSharedConfigPtr _config = KSharedConfig::openConfig(QStringLiteral("kcmknotessummaryrc"));

#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    mModelState = new KViewStateMaintainer<Akonadi::ETMViewStateSaver>(_config->group("CheckState"), this);
#else
    mModelState = new KViewStateMaintainer<Akonadi::ETMViewStateSaver>(_config->group("CheckState"), widget());
#endif
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

#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    Q_EMIT changed(false);
#else
    setNeedsSave(false);
#endif
}

void KCMKNotesSummary::save()
{
    storeFolders();
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    Q_EMIT changed(false);
#else
    setNeedsSave(false);
#endif
}

void KCMKNotesSummary::defaults()
{
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    Q_EMIT changed(true);
#else
    markAsChanged();
#endif
}

#include "kcmknotessummary.moc"
