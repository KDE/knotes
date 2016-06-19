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
#include <QIdentityProxyModel>
#include <QModelIndex>

class KRecursiveFilterProxyModel;
namespace Akonadi
{
class EntityTreeModel;
class ChangeRecorder;
class CollectionRequester;
class EntityTreeView;
}
class KJob;
class QPushButton;

class KNoteCollectionDisplayProxyModel : public QIdentityProxyModel
{
public:
    explicit KNoteCollectionDisplayProxyModel(QObject *parent = Q_NULLPTR);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    bool setData(const QModelIndex &index, const QVariant &_data, int role) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    QHash<Akonadi::Collection, bool> displayCollection() const;

private:
    QHash<Akonadi::Collection, bool> mDisplayCollection;
};


class KNoteCollectionConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KNoteCollectionConfigWidget(QWidget *parent = Q_NULLPTR);
    ~KNoteCollectionConfigWidget();

    void updateCollectionsRecursive();

    void save();
Q_SIGNALS:
    void emitChanged(bool);

private Q_SLOTS:
    void slotSelectAllCollections();
    void slotUnselectAllCollections();
    void slotCollectionsInserted();
    void slotModifyJobDone(KJob *job);
    void slotSetCollectionFilter(const QString &);
    void slotDataChanged();
    void slotRenameCollection();
    void slotUpdateButtons();
    void slotCollectionModifyFinished(KJob *);
private:
    void forceStatus(const QModelIndex &parent, bool status);
    Akonadi::EntityTreeView *mFolderView;
    Akonadi::EntityTreeModel *mModel;
    Akonadi::ChangeRecorder *mChangeRecorder;
    KRecursiveFilterProxyModel *mCollectionFilter;
    Akonadi::CollectionRequester *mDefaultSaveFolder;
    QPushButton *mRenameCollection;
    KNoteCollectionDisplayProxyModel *mDisplayNotifierProxyModel;
};

#endif // KNOTECOLLECTIONCONFIGWIDGET_H
