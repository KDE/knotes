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
    explicit KNotePrintSelectedNotesDialog(QWidget *parent = 0);
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
    QListWidget *mListNotes;
    QHash<Akonadi::Item::Id, KNote *> mNotes;
    KNotePrintSelectThemeComboBox *mTheme;
    QDialogButtonBox *mButtonBox;
    QPushButton *mUser1Button;
    bool mPreview;
};

#endif // KNOTEPRINTSELECTEDNOTESDIALOG_H
