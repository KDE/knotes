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
#ifndef KNOTECOLLECTIONCONFIGWIDGET_H
#define KNOTECOLLECTIONCONFIGWIDGET_H

#include <QWidget>
#include <AkonadiCore/Collection>
#include <QModelIndex>

class QItemSelectionModel;
class KRecursiveFilterProxyModel;
namespace Akonadi
{
class EntityTreeModel;
class ChangeRecorder;
class CollectionRequester;
class EntityTreeView;
}
class KCheckableProxyModel;
class KJob;
class QPushButton;

class KNoteCollectionConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KNoteCollectionConfigWidget(QWidget *parent = Q_NULLPTR);
    ~KNoteCollectionConfigWidget();

    void updateCollectionsRecursive(const QModelIndex &parent);

    void save();
Q_SIGNALS:
    void emitChanged(bool);

private Q_SLOTS:
    void slotSelectAllCollections();
    void slotUnselectAllCollections();
    void slotCollectionsInserted();
    void slotModifyJobDone(KJob *job);
    void slotUpdateCollectionStatus();
    void slotSetCollectionFilter(const QString &);
    void slotDataChanged();
    void slotRenameCollection();
    void slotUpdateButtons();
    void slotCollectionModifyFinished(KJob *);
private:
    void updateStatus(const QModelIndex &parent);
    void forceStatus(const QModelIndex &parent, bool status);
    Akonadi::EntityTreeView *mFolderView;
    QItemSelectionModel *mSelectionModel;
    Akonadi::EntityTreeModel *mModel;
    Akonadi::ChangeRecorder *mChangeRecorder;
    KCheckableProxyModel *mCheckProxy;
    KRecursiveFilterProxyModel *mCollectionFilter;
    Akonadi::CollectionRequester *mDefaultSaveFolder;
    QPushButton *mRenameCollection;
    bool mCanUpdateStatus;
};

#endif // KNOTECOLLECTIONCONFIGWIDGET_H
