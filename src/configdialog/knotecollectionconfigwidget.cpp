/*
   Copyright (C) 2013-2016 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "knotecollectionconfigwidget.h"
#include "attributes/showfoldernotesattribute.h"
#include <Akonadi/Notes/NoteUtils>
#include "notesharedglobalconfig.h"
#include "AkonadiWidgets/ManageAccountWidget"

#include <AkonadiCore/CollectionModifyJob>
#include <AkonadiCore/CollectionFilterProxyModel>
#include <KRecursiveFilterProxyModel>
#include <QInputDialog>

#include <AkonadiWidgets/CollectionRequester>
#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/EntityTreeModel>
#include <AkonadiCore/Collection>
#include <AkonadiWidgets/EntityTreeView>
#include <AkonadiCore/EntityDisplayAttribute>

#include <KMime/Message>

#include <KCheckableProxyModel>

#include <KLocalizedString>
#include <QPushButton>
#include <QLineEdit>
#include "knotes_debug.h"
#include <KMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

KNoteCollectionDisplayProxyModel::KNoteCollectionDisplayProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

QVariant KNoteCollectionDisplayProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::CheckStateRole) {
        if (index.isValid()) {

            const Akonadi::Collection collection =
                data(index, Akonadi::EntityTreeModel::CollectionRole).value<Akonadi::Collection>();
            if (mDisplayCollection.contains(collection)) {
                return mDisplayCollection.value(collection) ? Qt::Checked : Qt::Unchecked;
            } else {
                NoteShared::ShowFolderNotesAttribute *attr = collection.attribute<NoteShared::ShowFolderNotesAttribute>();
                if (attr) {
                    return Qt::Checked;
                }
                return Qt::Unchecked;
            }
        }
    }
    return QIdentityProxyModel::data(index, role);
}

bool KNoteCollectionDisplayProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole) {
        if (index.isValid()) {
            const Akonadi::Collection collection =
                data(index, Akonadi::EntityTreeModel::CollectionRole).value<Akonadi::Collection>();
            mDisplayCollection[collection] = (value == Qt::Checked);
            emit dataChanged(index, index);
            return true;
        }
    }

    return QIdentityProxyModel::setData(index, value, role);
}

Qt::ItemFlags KNoteCollectionDisplayProxyModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return QIdentityProxyModel::flags(index) | Qt::ItemIsUserCheckable;
    } else {
        return QIdentityProxyModel::flags(index);
    }
}

QHash<Akonadi::Collection, bool> KNoteCollectionDisplayProxyModel::displayCollection() const
{
    return mDisplayCollection;
}

KNoteCollectionConfigWidget::KNoteCollectionConfigWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);

    QTabWidget *tabWidget = new QTabWidget;
    mainLayout->addWidget(tabWidget);

    QWidget *collectionWidget = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    collectionWidget->setLayout(vbox);
    tabWidget->addTab(collectionWidget, i18n("Folders"));

    QLabel *label = new QLabel(i18n("Select which KNotes folders to show:"));
    vbox->addWidget(label);

    // Create a new change recorder.
    mChangeRecorder = new Akonadi::ChangeRecorder(this);
    mChangeRecorder->setMimeTypeMonitored(Akonadi::NoteUtils::noteMimeType());
    mChangeRecorder->fetchCollection(true);
    mChangeRecorder->setAllMonitored(true);

    mModel = new Akonadi::EntityTreeModel(mChangeRecorder, this);
    // Set the model to show only collections, not items.
    mModel->setItemPopulationStrategy(Akonadi::EntityTreeModel::NoItemPopulation);

    Akonadi::CollectionFilterProxyModel *mimeTypeProxy = new Akonadi::CollectionFilterProxyModel(this);
    mimeTypeProxy->setExcludeVirtualCollections(true);
    mimeTypeProxy->addMimeTypeFilters(QStringList() << Akonadi::NoteUtils::noteMimeType());
    mimeTypeProxy->setSourceModel(mModel);

    mDisplayNotifierProxyModel = new KNoteCollectionDisplayProxyModel(this);
    mDisplayNotifierProxyModel->setSourceModel(mimeTypeProxy);
    connect(mDisplayNotifierProxyModel, &KNoteCollectionDisplayProxyModel::dataChanged, this, &KNoteCollectionConfigWidget::slotDataChanged);

    connect(mModel, &Akonadi::EntityTreeModel::collectionTreeFetched, this, &KNoteCollectionConfigWidget::slotCollectionsInserted);

    mCollectionFilter = new KRecursiveFilterProxyModel(this);
    mCollectionFilter->setSourceModel(mDisplayNotifierProxyModel);
    mCollectionFilter->setDynamicSortFilter(true);
    mCollectionFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);

    QLineEdit *searchLine = new QLineEdit(this);
    searchLine->setPlaceholderText(i18n("Search..."));
    searchLine->setClearButtonEnabled(true);
    connect(searchLine, &QLineEdit::textChanged, this, &KNoteCollectionConfigWidget::slotSetCollectionFilter);

    vbox->addWidget(searchLine);

    mFolderView = new Akonadi::EntityTreeView(this);
    mFolderView->setDragEnabled(false);
    mFolderView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mFolderView->setAlternatingRowColors(true);
    vbox->addWidget(mFolderView);

    mFolderView->setModel(mCollectionFilter);
    connect(mFolderView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &KNoteCollectionConfigWidget::slotUpdateButtons);

    QHBoxLayout *hbox = new QHBoxLayout;
    vbox->addLayout(hbox);

    QPushButton *button = new QPushButton(i18n("&Select All"), this);
    connect(button, &QPushButton::clicked, this, &KNoteCollectionConfigWidget::slotSelectAllCollections);
    hbox->addWidget(button);

    button = new QPushButton(i18n("&Unselect All"), this);
    connect(button, &QPushButton::clicked, this, &KNoteCollectionConfigWidget::slotUnselectAllCollections);
    hbox->addWidget(button);
    hbox->addStretch(1);

    mRenameCollection = new QPushButton(i18n("Rename notes..."), this);
    connect(mRenameCollection, &QPushButton::clicked, this, &KNoteCollectionConfigWidget::slotRenameCollection);
    hbox->addWidget(mRenameCollection);

    vbox->addWidget(new QLabel(i18nc("@info", "Select the folder where the note will be saved:")));
    mDefaultSaveFolder = new Akonadi::CollectionRequester(Akonadi::Collection(NoteShared::NoteSharedGlobalConfig::self()->defaultFolder()));
    mDefaultSaveFolder->setMimeTypeFilter(QStringList() << Akonadi::NoteUtils::noteMimeType());
    mDefaultSaveFolder->setContentMimeTypes(QStringList() << QStringLiteral("application/x-vnd.akonadi.note")
                                            << QStringLiteral("text/x-vnd.akonadi.note")
                                            << QStringLiteral("inode/directory"));
    Akonadi::CollectionDialog::CollectionDialogOptions options;
    options |= Akonadi::CollectionDialog::AllowToCreateNewChildCollection;
    options |= Akonadi::CollectionDialog::KeepTreeExpanded;
    mDefaultSaveFolder->changeCollectionDialogOptions(options);
    connect(mDefaultSaveFolder, &Akonadi::CollectionRequester::collectionChanged, this, &KNoteCollectionConfigWidget::slotDataChanged);

    vbox->addWidget(mDefaultSaveFolder);

    QWidget *accountWidget = new QWidget;
    QVBoxLayout *vboxAccountWidget = new QVBoxLayout;
    accountWidget->setLayout(vboxAccountWidget);

    Akonadi::ManageAccountWidget *manageAccountWidget = new Akonadi::ManageAccountWidget(this);
    vboxAccountWidget->addWidget(manageAccountWidget);

    manageAccountWidget->setMimeTypeFilter(QStringList() << Akonadi::NoteUtils::noteMimeType());
    manageAccountWidget->setCapabilityFilter(QStringList() << QStringLiteral("Resource"));  // show only resources, no agents
    tabWidget->addTab(accountWidget, i18n("Accounts"));
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

    if (rows.size() != 1) {
        return;
    }

    QModelIndex idx = rows.at(0);

    QString title = idx.data().toString();

    Akonadi::Collection col = idx.data(Akonadi::EntityTreeModel::CollectionRole).value<Akonadi::Collection>();
    Q_ASSERT(col.isValid());
    if (!col.isValid()) {
        return;
    }

    bool ok;
    const QString name = QInputDialog::getText(this, i18n("Rename Notes"),
                         i18n("Name:"), QLineEdit::Normal, title, &ok);

    if (ok) {
        if (col.hasAttribute<Akonadi::EntityDisplayAttribute>() &&
                !col.attribute<Akonadi::EntityDisplayAttribute>()->displayName().isEmpty()) {
            col.attribute<Akonadi::EntityDisplayAttribute>()->setDisplayName(name);
        } else if (!name.isEmpty()) {
            col.setName(name);
        }

        Akonadi::CollectionModifyJob *job = new Akonadi::CollectionModifyJob(col, this);
        connect(job, &Akonadi::CollectionModifyJob::result, this, &KNoteCollectionConfigWidget::slotCollectionModifyFinished);
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

void KNoteCollectionConfigWidget::forceStatus(const QModelIndex &parent, bool status)
{
    const int nbCol = mDisplayNotifierProxyModel->rowCount(parent);
    for (int i = 0; i < nbCol; ++i) {
        const QModelIndex child = mDisplayNotifierProxyModel->index(i, 0, parent);
        mDisplayNotifierProxyModel->setData(child, status ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
        forceStatus(child, status);
    }
}

void KNoteCollectionConfigWidget::slotCollectionsInserted()
{
    mFolderView->expandAll();
}

void KNoteCollectionConfigWidget::save()
{
    updateCollectionsRecursive();
    Akonadi::Collection col = mDefaultSaveFolder->collection();
    if (col.isValid()) {
        NoteShared::NoteSharedGlobalConfig::self()->setDefaultFolder(col.id());
        NoteShared::NoteSharedGlobalConfig::self()->save();
    }
}

void KNoteCollectionConfigWidget::updateCollectionsRecursive()
{
    QHashIterator<Akonadi::Collection, bool> i(mDisplayNotifierProxyModel->displayCollection());
    while (i.hasNext()) {
        i.next();
        Akonadi::Collection collection = i.key();
        Akonadi::CollectionModifyJob *modifyJob = Q_NULLPTR;
        NoteShared::ShowFolderNotesAttribute *attr = collection.attribute<NoteShared::ShowFolderNotesAttribute>();
        const bool selected = i.value();
        if (selected && !attr) {
            attr = collection.attribute<NoteShared::ShowFolderNotesAttribute>(Akonadi::Collection::AddIfMissing);
            modifyJob = new Akonadi::CollectionModifyJob(collection);
            modifyJob->setProperty("AttributeAdded", true);
        } else if (!selected && attr) {
            collection.removeAttribute<NoteShared::ShowFolderNotesAttribute>();
            modifyJob = new Akonadi::CollectionModifyJob(collection);
            modifyJob->setProperty("AttributeAdded", false);
        }

        if (modifyJob) {
            connect(modifyJob, &Akonadi::CollectionModifyJob::finished, this, &KNoteCollectionConfigWidget::slotModifyJobDone);
        }
    }
}

void KNoteCollectionConfigWidget::slotModifyJobDone(KJob *job)
{
    Akonadi::CollectionModifyJob *modifyJob = qobject_cast<Akonadi::CollectionModifyJob *>(job);
    if (modifyJob && job->error()) {
        if (job->property("AttributeAdded").toBool()) {
            qCWarning(KNOTES_LOG) << "Failed to append ShowFolderNotesAttribute to collection"
                                  << modifyJob->collection().id() << ":"
                                  << job->errorString();
        } else {
            qCWarning(KNOTES_LOG) << "Failed to remove ShowFolderNotesAttribute from collection"
                                  << modifyJob->collection().id() << ":"
                                  << job->errorString();
        }
    }
}

