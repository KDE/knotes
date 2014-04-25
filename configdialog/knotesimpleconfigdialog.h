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


#ifndef KNoteSimpleConfigDialog_H
#define KNoteSimpleConfigDialog_H
#include "knotes_export.h"
#include <AkonadiCore/Item>
#include <KDialog>
class QTabWidget;
class KNoteEditorConfigWidget;
class KNoteDisplayConfigWidget;
class KNOTES_EXPORT KNoteSimpleConfigDialog : public KDialog
{
    Q_OBJECT
public:
    explicit KNoteSimpleConfigDialog( const QString &title, QWidget *parent=0 );
    ~KNoteSimpleConfigDialog();

    void load(Akonadi::Item &item, bool isRichText);
    void save(Akonadi::Item &item, bool &isRichText);

public Q_SLOTS:
    void slotUpdateCaption(const QString & name);

private:
    void readConfig();
    void writeConfig();
    QTabWidget *mTabWidget;
    KNoteEditorConfigWidget *mEditorConfigWidget;
    KNoteDisplayConfigWidget *mDisplayConfigWidget;
};

#endif // KNoteSimpleConfigDialog_H
