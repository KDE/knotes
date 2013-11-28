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

#include "knotesakonadiapp.h"
#include "knotesakonaditray.h"
#include "noteshared/akonadi/notesakonaditreemodel.h"
#include "knoteakonadinote.h"
#include "noteshared/akonadi/noteschangerecorder.h"

#include "noteshared/attributes/notelockattribute.h"
#include "noteshared/attributes/notedisplayattribute.h"
#include "noteshared/attributes/notealarmattribute.h"

#include <akonadi/control.h>
#include <Akonadi/ChangeRecorder>
#include <Akonadi/Collection>
#include <Akonadi/EntityTreeModel>
#include <Akonadi/Session>
#include <KMime/KMimeMessage>

#include <QTextEdit>
#include <QLineEdit>
#include <QDebug>
#include <QHash>

KNotesAkonadiApp::KNotesAkonadiApp(QWidget *parent)
    : QWidget(parent)
{
    Akonadi::Session *session = new Akonadi::Session( "KNotes Session", this );
    Akonadi::Control::widgetNeedsAkonadi(this);
    mNoteRecorder = new NoteShared::NotesChangeRecorder(this);
    mNoteRecorder->changeRecorder()->setSession(session);
    mTray = new KNotesAkonadiTray(mNoteRecorder->changeRecorder(), 0);

    mNoteTreeModel = new NoteShared::NotesAkonadiTreeModel(mNoteRecorder->changeRecorder(), this);

    connect( mNoteTreeModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
             SLOT(slotRowInserted(QModelIndex,int,int)));

    connect( mNoteRecorder->changeRecorder(), SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)), SLOT(slotItemChanged(Akonadi::Item,QSet<QByteArray>)));
    connect( mNoteRecorder->changeRecorder(), SIGNAL(itemRemoved(Akonadi::Item)), SLOT(slotItemRemoved(Akonadi::Item)) );
}

KNotesAkonadiApp::~KNotesAkonadiApp()
{
    qDeleteAll(mHashNotes);
}

void KNotesAkonadiApp::slotItemRemoved(const Akonadi::Item &item)
{
    qDebug()<<" note removed"<<item.id();
    if (mHashNotes.contains(item.id())) {
        delete mHashNotes.find(item.id()).value();
        mHashNotes.remove(item.id());
    }
}

void KNotesAkonadiApp::slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &set)
{
    if (mHashNotes.contains(item.id())) {
        qDebug()<<" item changed "<<item.id()<<" info "<<set.toList();
        KNoteAkonadiNote *note = mHashNotes.find(item.id()).value();
        if (set.contains("ATR:KJotsLockAttribute")) {
            note->setEnabled(!item.hasAttribute<NoteShared::NoteLockAttribute>());
        }
        if (set.contains("PLD:RFC822")) {
            KMime::Message::Ptr noteMessage = item.payload<KMime::Message::Ptr>();
            note->title()->setText(noteMessage->subject(false)->asUnicodeString());
            if ( noteMessage->contentType()->isHTMLText() ) {
                note->editor()->setAcceptRichText(true);
                note->editor()->setHtml(noteMessage->mainBodyPart()->decodedText());
            } else {
                note->editor()->setAcceptRichText(false);
                note->editor()->setPlainText(noteMessage->mainBodyPart()->decodedText());
            }
        }
        if (set.contains("ATR:NoteDisplayAttribute")) {

            //TODO
        }
        if (set.contains("ATR:NoteAlarmAttribute")) {
            //TODO
        }
    }
}

void KNotesAkonadiApp::slotRowInserted(const QModelIndex &parent, int start, int end)
{
    for ( int i = start; i <= end; ++i) {
        if ( mNoteTreeModel->hasIndex( i, 0, parent ) ) {
            const QModelIndex child = mNoteTreeModel->index( i, 0, parent );
            Akonadi::Item item =
                    mNoteTreeModel->data( child, Akonadi::EntityTreeModel::ItemRole ).value<Akonadi::Item>();
            if ( !item.hasPayload<KMime::Message::Ptr>() )
                continue;
            KMime::Message::Ptr noteMessage = item.payload<KMime::Message::Ptr>();
            KNoteAkonadiNote *note = new KNoteAkonadiNote(0);
            note->title()->setText(noteMessage->subject(false)->asUnicodeString());
            if ( noteMessage->contentType()->isHTMLText() ) {
                note->editor()->setAcceptRichText(true);
                note->editor()->setHtml(noteMessage->mainBodyPart()->decodedText());
            } else {
                note->editor()->setAcceptRichText(false);
                note->editor()->setPlainText(noteMessage->mainBodyPart()->decodedText());
            }
            if ( item.hasAttribute<NoteShared::NoteLockAttribute>() ) {
                note->setEnabled(false);
            }
            if ( item.hasAttribute<NoteShared::NoteDisplayAttribute>()) {
                //TODO add display attribute
                NoteShared::NoteDisplayAttribute *attr = item.attribute<NoteShared::NoteDisplayAttribute>();
                if (attr->isHidden()) {
                    note->hide();
                } else {
                    note->show();
                }
                note->resize(attr->size());
            } else {
                note->show();
            }
            if ( item.hasAttribute<NoteShared::NoteAlarmAttribute>()) {
                //TODO add alarm attribute
            }
            mHashNotes.insert(item.id(), note);
        }
    }
}
