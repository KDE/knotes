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
#include "knoteakonadinote.h"
#include "knoteschangerecorder.h"
#include <akonadi/control.h>
#include <Akonadi/ChangeRecorder>
#include <Akonadi/Item>
#include <Akonadi/Collection>
#include <Akonadi/EntityTreeModel>
#include <Akonadi/Session>

#include <QDebug>

KNotesAkonadiApp::KNotesAkonadiApp(QWidget *parent)
    : QWidget(parent)
{
    Akonadi::Session *session = new Akonadi::Session( "KNotes Session", this );
    Akonadi::Control::widgetNeedsAkonadi(this);
    mNoteRecorder = new KNotesChangeRecorder(this);
    mNoteRecorder->changeRecorder()->setSession(session);
    mTray = new KNotesAkonadiTray(mNoteRecorder->changeRecorder(), 0);

    Akonadi::EntityTreeModel *model = new Akonadi::EntityTreeModel( mNoteRecorder->changeRecorder(), this );
    model->setItemPopulationStrategy( Akonadi::EntityTreeModel::ImmediatePopulation );
    connect( model, SIGNAL(rowsInserted(QModelIndex,int,int)),
             SLOT(slotRowInserted(QModelIndex,int,int)));
    connect( model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
             SLOT(slotRowRemoved(QModelIndex,int,int)) );
    connect( model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             SLOT(slotDataChanged(QModelIndex,int,int)) );
}

KNotesAkonadiApp::~KNotesAkonadiApp()
{

}

void KNotesAkonadiApp::slotDataChanged(const QModelIndex & ,int,int)
{
    qDebug()<<" Data changed";
}

void KNotesAkonadiApp::slotRowInserted(const QModelIndex &,int,int)
{
    qDebug()<<" note inserted";
    KNoteAkonadiNote *note = new KNoteAkonadiNote(0);
    note->show();
}

void KNotesAkonadiApp::slotRowRemoved(const QModelIndex &,int,int)
{
    qDebug()<<" note removed";
}
