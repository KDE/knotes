/*
   Copyright (C) 2013-2016 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "kcmknotessummary.h"

#include <Akonadi/Notes/NoteUtils>

#include "PimCommon/CheckedCollectionWidget"

#include <AkonadiWidgets/ETMViewStateSaver>

#include <KAboutData>
#include <KAcceleratorManager>
#include <KCheckableProxyModel>
#include "knotes_kontact_plugin_debug.h"
#include <QDialog>
#include <KLocalizedString>
#include <QLineEdit>
#include <KSharedConfig>

#include <QCheckBox>
#include <QTreeView>
#include <QVBoxLayout>
#include <KConfigGroup>

extern "C"
{
    Q_DECL_EXPORT KCModule *create_knotessummary(QWidget *parent, const char *)
    {
        return new KCMKNotesSummary(parent);
    }
}

KCMKNotesSummary::KCMKNotesSummary(QWidget *parent)
    : KCModule(parent)
{
    initGUI();

    connect(mCheckedCollectionWidget->folderTreeView(), &QAbstractItemView::clicked,
            this, &KCMKNotesSummary::modified);

    KAcceleratorManager::manage(this);

    load();

    KAboutData *about = new KAboutData(QStringLiteral("kcmknotessummary"),
                                       i18n("kcmknotessummary"),
                                       QString(),
                                       i18n("Notes Summary Configuration Dialog"),
                                       KAboutLicense::GPL,
                                       i18n("Copyright © 2013-2016 Laurent Montel <montel@kde.org>"));
    about->addAuthor(ki18n("Laurent Montel").toString(), QString(), QStringLiteral("montel@kde.org"));
    setAboutData(about);
}

void KCMKNotesSummary::modified()
{
    Q_EMIT changed(true);
}

void KCMKNotesSummary::initGUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);

    mCheckedCollectionWidget = new PimCommon::CheckedCollectionWidget(Akonadi::NoteUtils::noteMimeType());
    layout->addWidget(mCheckedCollectionWidget);
}

void KCMKNotesSummary::initFolders()
{
    KSharedConfigPtr _config = KSharedConfig::openConfig(QStringLiteral("kcmknotessummaryrc"));

    mModelState =
        new KViewStateMaintainer<Akonadi::ETMViewStateSaver>(_config->group("CheckState"), this);
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
