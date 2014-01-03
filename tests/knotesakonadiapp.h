/*
  Copyright (c) 2013, 2014 Montel Laurent <montel.org>

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
namespace Akonadi {
}
namespace NoteShared {
class NotesChangeRecorder;
class NotesAkonadiTreeModel;
}
class QModelIndex;
class KNoteAkonadiNote;
class KNotesAkonadiApp : public QWidget
{
    Q_OBJECT
public:
    explicit KNotesAkonadiApp(QWidget *parent=0);
    ~KNotesAkonadiApp();

private Q_SLOTS:
    void slotRowInserted(const QModelIndex &, int, int end);    
    void slotItemRemoved(const Akonadi::Item &item);
    void slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &);

private:
    QHash<Akonadi::Item::Id, KNoteAkonadiNote*> mHashNotes;
    KNotesAkonadiTray *mTray;
    NoteShared::NotesChangeRecorder *mNoteRecorder;
    NoteShared::NotesAkonadiTreeModel *mNoteTreeModel;
};

#endif // KNOTESAKONADIAPP_H
