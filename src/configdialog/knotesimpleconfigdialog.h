/*
   Copyright (C) 2013-2019 Montel Laurent <montel@kde.org>

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
#ifndef KNoteSimpleConfigDialog_H
#define KNoteSimpleConfigDialog_H
#include "knotes_export.h"
#include <AkonadiCore/Item>
#include <QDialog>
class QTabWidget;
class KNoteEditorConfigWidget;
class KNoteDisplayConfigWidget;
class KNOTES_EXPORT KNoteSimpleConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KNoteSimpleConfigDialog(const QString &title, QWidget *parent = nullptr);
    ~KNoteSimpleConfigDialog();

    void load(Akonadi::Item &item, bool isRichText);
    void save(Akonadi::Item &item, bool &isRichText);

public Q_SLOTS:
    void slotUpdateCaption(const QString &name);

private:
    void readConfig();
    void writeConfig();
    QTabWidget *mTabWidget = nullptr;
    KNoteEditorConfigWidget *mEditorConfigWidget = nullptr;
    KNoteDisplayConfigWidget *mDisplayConfigWidget = nullptr;
};

#endif // KNoteSimpleConfigDialog_H
