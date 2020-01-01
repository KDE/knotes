/*
  This file is part of Kontact.

  Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>
  Copyright (C) 2014-2020 Laurent Montel <montel@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

  As a special exception, permission is given to link this program
  with any edition of Qt, and distribute the resulting executable,
  without including the source code for Qt in the source distribution.
*/

#ifndef SUMMARYWIDGET_H
#define SUMMARYWIDGET_H

#include <AkonadiCore/Item>

#include <KontactInterface/Summary>

#include <KViewStateMaintainer>

namespace NoteShared {
class NotesChangeRecorder;
class NotesAkonadiTreeModel;
}

namespace Akonadi {
class ETMViewStateSaver;
}

namespace KontactInterface {
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

#endif
