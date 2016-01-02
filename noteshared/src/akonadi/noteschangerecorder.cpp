/*
  Copyright (c) 2013-2016 Montel Laurent <montel@kde.org>

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

#include "noteschangerecorder.h"

#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/CollectionFetchScope>
#include <AkonadiCore/ItemFetchScope>
#include <Akonadi/Notes/NoteUtils>

#include "attributes/notealarmattribute.h"
#include "attributes/notelockattribute.h"
#include "attributes/notedisplayattribute.h"

using namespace NoteShared;

class NoteShared::NotesChangeRecorderPrivate
{
public:
    NotesChangeRecorderPrivate()
        : mChangeRecorder(Q_NULLPTR)
    {

    }

    Akonadi::ChangeRecorder *mChangeRecorder;
};

NotesChangeRecorder::NotesChangeRecorder(QObject *parent)
    : QObject(parent),
      d(new NoteShared::NotesChangeRecorderPrivate)
{
    Akonadi::ItemFetchScope scope;
    scope.fetchFullPayload(true);   // Need to have full item when adding it to the internal data structure
    scope.fetchAttribute< NoteShared::NoteLockAttribute >();
    scope.fetchAttribute< NoteShared::NoteDisplayAttribute >();
    scope.fetchAttribute< NoteShared::NoteAlarmAttribute >();

    d->mChangeRecorder = new Akonadi::ChangeRecorder(this);
    d->mChangeRecorder->setItemFetchScope(scope);
    d->mChangeRecorder->fetchCollection(true);
    d->mChangeRecorder->fetchCollectionStatistics(true);
    d->mChangeRecorder->setCollectionMonitored(Akonadi::Collection::root());
    d->mChangeRecorder->collectionFetchScope().setIncludeStatistics(true);
    d->mChangeRecorder->setMimeTypeMonitored(Akonadi::NoteUtils::noteMimeType());
}

NotesChangeRecorder::~NotesChangeRecorder()
{
    delete d;
}

Akonadi::ChangeRecorder *NotesChangeRecorder::changeRecorder() const
{
    return d->mChangeRecorder;
}
