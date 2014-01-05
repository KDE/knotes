/*
  Copyright (c) 2013, 2014 Montel Laurent <montel@kde.org>

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

#ifndef KNOTESELECTEDNOTESDIALOG_H
#define KNOTESELECTEDNOTESDIALOG_H

#include <KDialog>
#include <Akonadi/Item>
class QListWidget;
class KNote;
class KNoteSelectedNotesDialog : public KDialog
{
    Q_OBJECT
public:
    explicit KNoteSelectedNotesDialog(QWidget *parent=0);
    ~KNoteSelectedNotesDialog();

    void setNotes(const QHash<Akonadi::Item::Id, KNote*> &notes);

    QStringList selectedNotes() const;

private Q_SLOTS:
    void slotSelectionChanged();

private:
    enum listViewData {
        AkonadiId = Qt::UserRole + 1
    };

    void writeConfig();
    void readConfig();
    QListWidget *mListNotes;
    QHash<Akonadi::Item::Id, KNote*> mNotes;
};

#endif // KNOTESELECTEDNOTESDIALOG_H
