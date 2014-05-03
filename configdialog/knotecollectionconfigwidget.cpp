/*
    Copyright (c) 2013 Laurent Montel <montel@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "knotecollectionconfigwidget.h"
#include "noteshared/attributes/showfoldernotesattribute.h"
#include "akonadi_next/note.h"
#include "notesharedglobalconfig.h"

#include <AkonadiCore/CollectionModifyJob>
#include <AkonadiCore/CollectionFilterProxyModel>
#include <KRecursiveFilterProxyModel>
#include <KInputDialog>

#include <AkonadiWidgets/CollectionRequester>
#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/EntityTreeModel>
#include <AkonadiCore/Collection>
#include <AkonadiWidgets/EntityTreeView>
#include <AkonadiCore/EntityDisplayAttribute>
#include <AkonadiCore/CollectionModifyJob>

#include <KMime/Message>

#include <KCheckableProxyModel>

#include <KLocalizedString>
#include <QPushButton>
#include <KLineEdit>
#include <QDebug>
#include <KMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QLabel>
#include <QTimer>

KNoteCollectionConfigWidget::KNoteCollectionConfigWidget(QWidget *parent)
    : QWidget(parent),
      mCanUpdateStatus(false)
{
    QVBoxLayout *vbox = new QVBoxLayout;

    QLabel *label = new QLabel(i18n("Select which KNotes folders to show:"));
    vbox->addWidget(label);

    // Create a new change recorder.
    mChangeRecorder = new Akonadi::ChangeRecorder( this );
    mChangeRecorder->setMimeTypeMonitored( Akonotes::Note::mimeType() );
    mChangeRecorder->fetchCollection( true );
    mChangeRecorder->setAllMonitored( true );

    mModel = new Akonadi::EntityTreeModel( mChangeRecorder, this );
    // Set the model to show only collections, not items.
    mModel->setItemPopulationStrategy( Akonadi::EntityTreeModel::NoItemPopulation );

    Akonadi::CollectionFilterProxyModel *mimeTypeProxy = new Akonadi::CollectionFilterProxyModel( this );
    mimeTypeProxy->setExcludeVirtualCollections( true );
    mimeTypeProxy->addMimeTypeFilters( QStringList() << Akonotes::Note::mimeType() );
    mimeTypeProxy->setSourceModel( mModel );

    // Create the Check proxy model.
    mSelectionModel = new QItemSelectionModel( mimeTypeProxy );
    mCheckProxy = new KCheckableProxyModel( this );
    mCheckProxy->setSelectionModel( mSelectionModel );
    mCheckProxy->setSourceModel( mimeTypeProxy );

    connect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(slotCollectionsInserted(QModelIndex,int,int)));

    connect(mCheckProxy, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(slotDataChanged()));
    mCollectionFilter = new KRecursiveFilterProxyModel(this);
    mCollectionFilter->setSourceModel(mCheckProxy);
    mCollectionFilter->setDynamicSortFilter(true);
    mCollectionFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);


    KLineEdit *searchLine = new KLineEdit(this);
    searchLine->setPlaceholderText(i18n("Search..."));
    searchLine->setClearButtonShown(true);
    connect(searchLine, SIGNAL(textChanged(QString)),
            this, SLOT(slotSetCollectionFilter(QString)));

    vbox->addWidget(searchLine);

    mFolderView = new Akonadi::EntityTreeView(this);
    mFolderView->setDragEnabled(false);
    mFolderView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mFolderView->setAlternatingRowColors(true);
    vbox->addWidget(mFolderView);

    mFolderView->setModel( mCollectionFilter );
    connect(mFolderView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(slotUpdateButtons()));

    QHBoxLayout *hbox = new QHBoxLayout;
    vbox->addLayout(hbox);

    QPushButton *button = new QPushButton(i18n("&Select All"), this);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(slotSelectAllCollections()));
    hbox->addWidget(button);

    button = new QPushButton(i18n("&Unselect All"), this);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(slotUnselectAllCollections()));
    hbox->addWidget(button);
    hbox->addStretch(1);

    mRenameCollection = new QPushButton(i18n("Rename notes..."), this);
    connect(mRenameCollection, SIGNAL(clicked(bool)), this, SLOT(slotRenameCollection()));
    hbox->addWidget(mRenameCollection);


    vbox->addWidget(new QLabel(i18nc( "@info", "Select the folder where the note will be saved:" )));
    mDefaultSaveFolder = new Akonadi::CollectionRequester(Akonadi::Collection(NoteShared::NoteSharedGlobalConfig::self()->defaultFolder()));
    mDefaultSaveFolder->setMimeTypeFilter(QStringList() << Akonotes::Note::mimeType());

    vbox->addWidget(mDefaultSaveFolder);

    setLayout(vbox);
    QTimer::singleShot(1000, this, SLOT(slotUpdateCollectionStatus()));
    slotUpdateButtons();
}

KNoteCollectionConfigWidget::~KNoteCollectionConfigWidget()
{

}

void KNoteCollectionConfigWidget::slotUpdateButtons()
{
    mRenameCollection->setEnabled(mFolderView->selectionModel()->hasSelection());
}

void KNoteCollectionConfigWidget::slotRenameCollection()
{
    const QModelIndexList rows = mFolderView->selectionModel()->selectedRows();

    if ( rows.size() != 1 )
      return;

    QModelIndex idx = rows.at( 0 );

    QString title = idx.data().toString();

    Akonadi::Collection col = idx.data( Akonadi::EntityTreeModel::CollectionRole ).value<Akonadi::Collection>();
    Q_ASSERT( col.isValid() );
    if (!col.isValid())
      return;

    bool ok;
    const QString name = KInputDialog::getText( i18n( "Rename Notes" ),
        i18n( "Name:" ), title, &ok, this );

    if ( ok ) {
        if ( col.hasAttribute<Akonadi::EntityDisplayAttribute>() &&
             !col.attribute<Akonadi::EntityDisplayAttribute>()->displayName().isEmpty() ) {
            col.attribute<Akonadi::EntityDisplayAttribute>()->setDisplayName( name );
        } else if ( !name.isEmpty() ) {
            col.setName( name );
        }

        Akonadi::CollectionModifyJob *job = new Akonadi::CollectionModifyJob(col, this);
        connect( job, SIGNAL(result(KJob*)), SLOT(slotCollectionModifyFinished(KJob*)) );
        job->start();
    }
}

void KNoteCollectionConfigWidget::slotCollectionModifyFinished(KJob *job)
{
    if (job->error()) {
        KMessageBox::error(this, i18n("An error was occurred during renaming: %1", job->errorString()), i18n("Rename note"));
    }
}

void KNoteCollectionConfigWidget::slotDataChanged()
{
    Q_EMIT emitChanged(true);
}

void KNoteCollectionConfigWidget::slotSetCollectionFilter(const QString &filter)
{
    mCollectionFilter->setFilterWildcard(filter);
    mFolderView->expandAll();
}

void KNoteCollectionConfigWidget::slotUpdateCollectionStatus()
{
    mCanUpdateStatus = true;
    updateStatus(QModelIndex());
}

void KNoteCollectionConfigWidget::slotSelectAllCollections()
{
    forceStatus(QModelIndex(), true);
    Q_EMIT emitChanged(true);
}

void KNoteCollectionConfigWidget::slotUnselectAllCollections()
{
    forceStatus(QModelIndex(), false);
    Q_EMIT emitChanged(true);
}

void KNoteCollectionConfigWidget::updateStatus(const QModelIndex &parent)
{
    if (!mCanUpdateStatus)
        return;

    const int nbCol = mCheckProxy->rowCount( parent );
    for ( int i = 0; i < nbCol; ++i ) {
        const QModelIndex child = mCheckProxy->index( i, 0, parent );

        const Akonadi::Collection collection =
                mCheckProxy->data( child, Akonadi::EntityTreeModel::CollectionRole ).value<Akonadi::Collection>();

        NoteShared::ShowFolderNotesAttribute *attr = collection.attribute<NoteShared::ShowFolderNotesAttribute>();
        if (attr) {
            mCheckProxy->setData( child, Qt::Checked, Qt::CheckStateRole );
        }
        updateStatus( child );
    }
}

void KNoteCollectionConfigWidget::forceStatus(const QModelIndex &parent, bool status)
{
    const int nbCol = mCheckProxy->rowCount( parent );
    for ( int i = 0; i < nbCol; ++i ) {
        const QModelIndex child = mCheckProxy->index( i, 0, parent );
        mCheckProxy->setData( child, status ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole );
        forceStatus( child, status );
    }
}

void KNoteCollectionConfigWidget::slotCollectionsInserted(const QModelIndex &, int, int)
{
    mFolderView->expandAll();
}

void KNoteCollectionConfigWidget::save()
{
    updateCollectionsRecursive(QModelIndex());
    Akonadi::Collection col = mDefaultSaveFolder->collection();
    if (col.isValid()) {
        NoteShared::NoteSharedGlobalConfig::self()->setDefaultFolder(col.id());
        NoteShared::NoteSharedGlobalConfig::self()->writeConfig();
    }
}

void KNoteCollectionConfigWidget::updateCollectionsRecursive(const QModelIndex &parent)
{
    const int nbCol = mCheckProxy->rowCount( parent );
    for ( int i = 0; i < nbCol; ++i ) {
        const QModelIndex child = mCheckProxy->index( i, 0, parent );

        Akonadi::Collection collection =
                mCheckProxy->data( child, Akonadi::EntityTreeModel::CollectionRole ).value<Akonadi::Collection>();

        NoteShared::ShowFolderNotesAttribute *attr = collection.attribute<NoteShared::ShowFolderNotesAttribute>();
        Akonadi::CollectionModifyJob *modifyJob = 0;
        const bool selected = (mCheckProxy->data( child, Qt::CheckStateRole ).value<int>() != 0);
        if (selected && !attr) {
            attr = collection.attribute<NoteShared::ShowFolderNotesAttribute>(Akonadi::Entity::AddIfMissing);
            modifyJob = new Akonadi::CollectionModifyJob(collection);
            modifyJob->setProperty("AttributeAdded", true);
        } else if (!selected && attr) {
            collection.removeAttribute<NoteShared::ShowFolderNotesAttribute>();
            modifyJob = new Akonadi::CollectionModifyJob(collection);
            modifyJob->setProperty("AttributeAdded", false);
        }

        if (modifyJob) {
            connect(modifyJob, SIGNAL(finished(KJob*)), SLOT(slotModifyJobDone(KJob*)));
        }
        updateCollectionsRecursive(child);
    }
}

void KNoteCollectionConfigWidget::slotModifyJobDone(KJob* job)
{
    Akonadi::CollectionModifyJob *modifyJob = qobject_cast<Akonadi::CollectionModifyJob*>(job);
    if (modifyJob && job->error()) {
        if (job->property("AttributeAdded").toBool()) {
            qWarning() << "Failed to append NewMailNotifierAttribute to collection"
                       << modifyJob->collection().id() << ":"
                       << job->errorString();
        } else {
            qWarning() << "Failed to remove NewMailNotifierAttribute from collection"
                       << modifyJob->collection().id() << ":"
                       << job->errorString();
        }
    }
}


