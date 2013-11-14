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

#include "knoteschangerecorder.h"
#include <Akonadi/ChangeRecorder>
#include <Akonadi/CollectionFetchScope>
#include "akonadi_next/note.h"

KNotesChangeRecorder::KNotesChangeRecorder(QObject *parent)
    : QObject(parent)
{
    mChangeRecorder = new Akonadi::ChangeRecorder( this );
    mChangeRecorder->setMimeTypeMonitored( Akonotes::Note::mimeType() );
    mChangeRecorder->fetchCollectionStatistics( true );
    mChangeRecorder->setAllMonitored( true );
    mChangeRecorder->collectionFetchScope().setIncludeStatistics( true );
}

Akonadi::ChangeRecorder *KNotesChangeRecorder::changeRecorder() const
{
    return mChangeRecorder;
}
