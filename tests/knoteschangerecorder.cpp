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
#include <Akonadi/ItemFetchScope>
#include "akonadi_next/note.h"
#include "noteshared/attributes/notealarmattribute.h"
#include "noteshared/attributes/notelockattribute.h"
#include "noteshared/attributes/notedisplayattribute.h"

KNotesChangeRecorder::KNotesChangeRecorder(QObject *parent)
    : QObject(parent)
{
    Akonadi::ItemFetchScope scope;
    scope.fetchFullPayload( true ); // Need to have full item when adding it to the internal data structure
    scope.fetchAttribute< NoteShared::NoteLockAttribute >();
    scope.fetchAttribute< NoteShared::NoteDisplayAttribute >();
    scope.fetchAttribute< NoteShared::NoteAlarmAttribute >();

    mChangeRecorder = new Akonadi::ChangeRecorder( this );
    mChangeRecorder->setItemFetchScope( scope );
    mChangeRecorder->fetchCollection( true );
    mChangeRecorder->fetchCollectionStatistics( true );
    mChangeRecorder->setCollectionMonitored( Akonadi::Collection::root() );
    mChangeRecorder->collectionFetchScope().setIncludeStatistics( true );    
    mChangeRecorder->setMimeTypeMonitored( Akonotes::Note::mimeType() );
}

Akonadi::ChangeRecorder *KNotesChangeRecorder::changeRecorder() const
{
    return mChangeRecorder;
}
