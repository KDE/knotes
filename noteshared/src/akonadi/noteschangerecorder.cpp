/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "noteschangerecorder.h"

#include <Akonadi/ChangeRecorder>
#include <Akonadi/CollectionFetchScope>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/NoteUtils>

#include "attributes/notealarmattribute.h"
#include "attributes/notedisplayattribute.h"
#include "attributes/notelockattribute.h"

using namespace NoteShared;

NotesChangeRecorder::NotesChangeRecorder(QObject *parent)
    : QObject(parent)
    , mChangeRecorder(new Akonadi::ChangeRecorder(this))
{
    Akonadi::ItemFetchScope scope;
    scope.fetchFullPayload(true); // Need to have full item when adding it to the internal data structure
    scope.fetchAttribute<NoteShared::NoteLockAttribute>();
    scope.fetchAttribute<NoteShared::NoteDisplayAttribute>();
    scope.fetchAttribute<NoteShared::NoteAlarmAttribute>();

    mChangeRecorder->setItemFetchScope(scope);
    mChangeRecorder->fetchCollection(true);
    mChangeRecorder->fetchCollectionStatistics(true);
    mChangeRecorder->setCollectionMonitored(Akonadi::Collection::root());
    mChangeRecorder->collectionFetchScope().setIncludeStatistics(true);
    mChangeRecorder->setMimeTypeMonitored(Akonadi::NoteUtils::noteMimeType());
}

NotesChangeRecorder::~NotesChangeRecorder() = default;

Akonadi::ChangeRecorder *NotesChangeRecorder::changeRecorder() const
{
    return mChangeRecorder;
}
