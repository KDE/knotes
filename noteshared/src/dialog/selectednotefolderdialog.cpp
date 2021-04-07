/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "selectednotefolderdialog.h"

#include <Akonadi/Notes/NoteUtils>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

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

void SelectedNotefolderDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), "SelectedNotefolderDialog");
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void SelectedNotefolderDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), "SelectedNotefolderDialog");
    group.writeEntry("Size", size());
    group.sync();
}
