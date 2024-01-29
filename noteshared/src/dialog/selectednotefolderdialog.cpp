/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "selectednotefolderdialog.h"

#include <Akonadi/NoteUtils>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KWindowConfig>
#include <QWindow>

using namespace NoteShared;

SelectedNotefolderDialog::SelectedNotefolderDialog(QWidget *parent)
    : Akonadi::CollectionDialog(parent)
{
    const QStringList mimeTypes(Akonadi::NoteUtils::noteMimeType());
    setMimeTypeFilter(mimeTypes);
    // setAccessRightsFilter( Akonadi::Collection::CanCreateItem );
    setWindowTitle(i18nc("@title:window", "Select Note Folder"));
    setDescription(i18nc("@info", "Select the folder where the note will be saved:"));
    changeCollectionDialogOptions(Akonadi::CollectionDialog::KeepTreeExpanded);
    setUseFolderByDefault(false);
    readConfig();
}

SelectedNotefolderDialog::~SelectedNotefolderDialog()
{
    writeConfig();
}
namespace
{
static const char mySelectedNotefolderDialogName[] = "SelectedNotefolderDialog";
}
void SelectedNotefolderDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(600, 400));
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(mySelectedNotefolderDialogName));
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void SelectedNotefolderDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(mySelectedNotefolderDialogName));
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}

#include "moc_selectednotefolderdialog.cpp"
