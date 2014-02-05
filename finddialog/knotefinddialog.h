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

#ifndef KNOTEFINDDIALOG_H
#define KNOTEFINDDIALOG_H

#include <KDialog>
#include "knotes_export.h"
class QPushButton;
class KLineEdit;
class QListWidget;
class KNoteFindWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KNoteFindWidget(QWidget *parent=0);
    ~KNoteFindWidget();

private Q_SLOTS:
    void slotTextChanged(const QString &);
    void slotSearchNote();
private:
    KLineEdit *mSearchLineEdit;
    QPushButton *mSearchButton;
    QListWidget *mNoteList;
};

class KNOTES_EXPORT KNoteFindDialog : public KDialog
{
    Q_OBJECT
public:
    explicit KNoteFindDialog(QWidget *parent=0);
    ~KNoteFindDialog();

private:
    KNoteFindWidget *mNoteFindWidget;
};

#endif // KNOTEFINDDIALOG_H
