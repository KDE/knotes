/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <AkonadiCore/Collection>
#include <QIdentityProxyModel>
#include <QModelIndex>
#include <QWidget>
class QSortFilterProxyModel;
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
    Q_OBJECT
public:
    explicit KNoteCollectionDisplayProxyModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &_data, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QHash<Akonadi::Collection, bool> displayCollection() const;

private:
    QHash<Akonadi::Collection, bool> mDisplayCollection;
};

class KNoteCollectionConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KNoteCollectionConfigWidget(QWidget *parent = nullptr);
    ~KNoteCollectionConfigWidget() override;

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
    Akonadi::EntityTreeView *mFolderView = nullptr;
    Akonadi::EntityTreeModel *mModel = nullptr;
    Akonadi::ChangeRecorder *mChangeRecorder = nullptr;
    QSortFilterProxyModel *mCollectionFilter = nullptr;
    Akonadi::CollectionRequester *mDefaultSaveFolder = nullptr;
    QPushButton *mRenameCollection = nullptr;
    KNoteCollectionDisplayProxyModel *mDisplayNotifierProxyModel = nullptr;
};

