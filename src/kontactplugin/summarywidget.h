/*
  This file is part of Kontact.

  SPDX-FileCopyrightText: 2003 Tobias Koenig <tokoe@kde.org>
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#pragma once

#include <AkonadiCore/Item>

#include <KontactInterface/Summary>

#include <KViewStateMaintainer>

namespace NoteShared
{
class NotesChangeRecorder;
class NotesAkonadiTreeModel;
}

namespace Akonadi
{
class ETMViewStateSaver;
}

namespace KontactInterface
{
class Plugin;
}

class KCheckableProxyModel;

class QGridLayout;
class QItemSelectionModel;
class QLabel;

class KNotesSummaryWidget : public KontactInterface::Summary
{
    Q_OBJECT
public:
    KNotesSummaryWidget(KontactInterface::Plugin *plugin, QWidget *parent);
    ~KNotesSummaryWidget() override;

    void updateSummary(bool force = false) override;
    QStringList configModules() const override;

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;

private Q_SLOTS:
    void updateFolderList();
    void slotSelectNote(const QString &note);
    void slotPopupMenu(const QString &);

private:
    void deleteNote(const QString &note);
    void displayNotes(const QModelIndex &parent, int &counter);
    void createNote(const Akonadi::Item &item, int counter);
    QPixmap mDefaultPixmap;
    QGridLayout *mLayout = nullptr;
    KontactInterface::Plugin *mPlugin = nullptr;
    QList<QLabel *> mLabels;
    QPixmap mPixmap;
    NoteShared::NotesChangeRecorder *mNoteRecorder = nullptr;
    NoteShared::NotesAkonadiTreeModel *mNoteTreeModel = nullptr;
    QItemSelectionModel *mSelectionModel = nullptr;
    KCheckableProxyModel *mModelProxy = nullptr;
    KViewStateMaintainer<Akonadi::ETMViewStateSaver> *mModelState = nullptr;
    bool mInProgress = false;
};

