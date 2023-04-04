/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "kcmutils_version.h"
#include <KCModule>
#include <KViewStateMaintainer>
namespace Akonadi
{
class ETMViewStateSaver;
}

namespace PimCommon
{
class CheckedCollectionWidget;
}

class KCMKNotesSummary : public KCModule
{
    Q_OBJECT

public:
    explicit KCMKNotesSummary(QObject *parent, const KPluginMetaData &data, const QVariantList &args = QVariantList());

    void load() override;
    void save() override;
    void defaults() override;

private Q_SLOTS:
    void modified();

private:
    void initGUI();
    void initFolders();
    void loadFolders();
    void storeFolders();

    PimCommon::CheckedCollectionWidget *mCheckedCollectionWidget = nullptr;
    KViewStateMaintainer<Akonadi::ETMViewStateSaver> *mModelState = nullptr;
};
