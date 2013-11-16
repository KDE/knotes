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

#ifndef KNOTESAKONADIAPP_H
#define KNOTESAKONADIAPP_H

#include <QWidget>
#include <Akonadi/Item>
class KNotesAkonadiTray;
class KNotesChangeRecorder;
namespace Akonadi {
class Collection;
}
class QModelIndex;
class KNotesAkonadiTreeModel;
class KNoteAkonadiNote;
class KNotesAkonadiApp : public QWidget
{
    Q_OBJECT
public:
    explicit KNotesAkonadiApp(QWidget *parent=0);
    ~KNotesAkonadiApp();

private Q_SLOTS:
    void slotRowInserted(const QModelIndex &, int, int end);
    void slotRowRemoved(const QModelIndex &parent, int start, int end);
    void slotDataChanged(const QModelIndex &, const QModelIndex &);

private:
    QHash<Akonadi::Item::Id, KNoteAkonadiNote*> mHashNotes;
    KNotesAkonadiTray *mTray;
    KNotesChangeRecorder *mNoteRecorder;
    KNotesAkonadiTreeModel *mNoteTreeModel;
};

#endif // KNOTESAKONADIAPP_H
