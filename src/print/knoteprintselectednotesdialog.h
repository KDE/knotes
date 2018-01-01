/*
   Copyright (C) 2013-2018 Montel Laurent <montel@kde.org>

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

#ifndef KNOTEPRINTSELECTEDNOTESDIALOG_H
#define KNOTEPRINTSELECTEDNOTESDIALOG_H

#include <QDialog>
#include <AkonadiCore/Item>
class QListWidget;
class KNote;
class KNotePrintObject;
class KNotePrintSelectThemeComboBox;
class QDialogButtonBox;
class QPushButton;
class KNotePrintSelectedNotesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KNotePrintSelectedNotesDialog(QWidget *parent = nullptr);
    ~KNotePrintSelectedNotesDialog();

    void setNotes(const QHash<Akonadi::Item::Id, KNote *> &notes);

    QList<KNotePrintObject *> selectedNotes() const;
    QString selectedTheme() const;

    bool preview() const;

private Q_SLOTS:
    void slotPreview();
    void slotSelectionChanged();

private:
    enum listViewData {
        AkonadiId = Qt::UserRole + 1
    };

    void writeConfig();
    void readConfig();
    QListWidget *mListNotes = nullptr;
    QHash<Akonadi::Item::Id, KNote *> mNotes;
    KNotePrintSelectThemeComboBox *mTheme = nullptr;
    QDialogButtonBox *mButtonBox = nullptr;
    QPushButton *mUser1Button = nullptr;
    bool mPreview = false;
};

#endif // KNOTEPRINTSELECTEDNOTESDIALOG_H
