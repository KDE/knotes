/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "noteschangerecorder.h"

#include <Akonadi/Notes/NoteUtils>
#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/CollectionFetchScope>
#include <AkonadiCore/ItemFetchScope>

#include "attributes/notealarmattribute.h"
#include "attributes/notedisplayattribute.h"
#include "attributes/notelockattribute.h"

using namespace NoteShared;

class NoteShared::NotesChangeRecorderPrivate
{
public:
    NotesChangeRecorderPrivate()
    {
    }

    Akonadi::ChangeRecorder *mChangeRecorder = nullptr;
};

NotesChangeRecorder::NotesChangeRecorder(QObject *parent)
    : QObject(parent)
    , d(new NoteShared::NotesChangeRecorderPrivate)
{
    Akonadi::ItemFetchScope scope;
    scope.fetchFullPayload(true); // Need to have full item when adding it to the internal data structure
    scope.fetchAttribute<NoteShared::NoteLockAttribute>();
    scope.fetchAttribute<NoteShared::NoteDisplayAttribute>();
    scope.fetchAttribute<NoteShared::NoteAlarmAttribute>();

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
