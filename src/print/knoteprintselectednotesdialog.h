/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <AkonadiCore/Item>
#include <QDialog>
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
    ~KNotePrintSelectedNotesDialog() override;

    void setNotes(const QHash<Akonadi::Item::Id, KNote *> &notes);

    Q_REQUIRED_RESULT QList<KNotePrintObject *> selectedNotes() const;
    Q_REQUIRED_RESULT QString selectedTheme() const;

    Q_REQUIRED_RESULT bool preview() const;

private Q_SLOTS:
    void slotPreview();
    void slotSelectionChanged();

private:
    enum listViewData { AkonadiId = Qt::UserRole + 1 };

    void writeConfig();
    void readConfig();
    QListWidget *mListNotes = nullptr;
    QHash<Akonadi::Item::Id, KNote *> mNotes;
    KNotePrintSelectThemeComboBox *mTheme = nullptr;
    QDialogButtonBox *mButtonBox = nullptr;
    QPushButton *mUser1Button = nullptr;
    bool mPreview = false;
};

