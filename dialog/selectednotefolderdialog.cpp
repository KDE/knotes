/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "selectednotefolderdialog.h"

#include "akonadi_next/note.h"

#include <KLocale>
#include <KGlobal>
#include <KConfigGroup>

SelectedNotefolderDialog::SelectedNotefolderDialog(QWidget *parent)
    : Akonadi::CollectionDialog (parent)
{
    const QStringList mimeTypes( Akonotes::Note::mimeType() );
    setMimeTypeFilter( mimeTypes );
    setAccessRightsFilter( Akonadi::Collection::CanCreateItem );
    setCaption( i18nc( "@title:window", "Select Note Folder" ) );
    setDescription( i18nc( "@info",
                       "Select the folder where the note will be saved:" ) );
    changeCollectionDialogOptions( Akonadi::CollectionDialog::KeepTreeExpanded );
    setUseFolderByDefault(false);
    readConfig();
}

SelectedNotefolderDialog::~SelectedNotefolderDialog()
{
    writeConfig();
}

void SelectedNotefolderDialog::readConfig()
{
    KConfigGroup group( KGlobal::config(), "SelectedNotefolderDialog" );
    const QSize size = group.readEntry( "Size", QSize(600, 400) );
    if ( size.isValid() ) {
        resize( size );
    }
}

void SelectedNotefolderDialog::writeConfig()
{
    KConfigGroup group( KGlobal::config(), "SelectedNotefolderDialog" );
    group.writeEntry( "Size", size() );
    group.sync();
}
