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

#ifndef KNOTESNEPOMUKDEBUGDIALOG_H
#define KNOTESNEPOMUKDEBUGDIALOG_H

#include <KDialog>
namespace PimCommon {
class AkonadiResultListView;
class PlainTextEditorWidget;
}
class KJob;
class QModelIndex;
class KNotesNepomukDebugDialog : public KDialog
{
    Q_OBJECT
public:
    explicit KNotesNepomukDebugDialog(const QStringList &listUid, QWidget *parent=0);
    ~KNotesNepomukDebugDialog();

private slots:
    void slotSearchInfoWithNepomuk();
    void slotItemFetched(KJob *job);
    void slotShowItem(const QModelIndex &index);

private:
    void readConfig();
    void writeConfig();
    PimCommon::AkonadiResultListView *mListView;
    PimCommon::PlainTextEditorWidget *mResult;
};

#endif // KNOTESNEPOMUKDEBUGDIALOG_H
