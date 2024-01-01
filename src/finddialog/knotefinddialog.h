/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "knotes_export.h"
#include <Akonadi/Item>
#include <QDialog>
class QPushButton;
class QLineEdit;
class QListWidgetItem;
class QListWidget;
class QLabel;
namespace NoteShared
{
class NoteListWidget;
}
class KNoteFindWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KNoteFindWidget(QWidget *parent = nullptr);
    ~KNoteFindWidget() override;

    void setExistingNotes(const QHash<Akonadi::Item::Id, Akonadi::Item> &notes);

Q_SIGNALS:
    void noteSelected(Akonadi::Item::Id);

private Q_SLOTS:
    void slotTextChanged(const QString &);
    void slotSearchNote();
    void slotItemDoubleClicked(QListWidgetItem *);

private:
    void customContextMenuRequested(const QPoint &);
    QHash<Akonadi::Item::Id, Akonadi::Item> mNotes;
    QLabel *const mResultSearch;
    QLineEdit *const mSearchLineEdit;
    QPushButton *const mSearchButton;
    NoteShared::NoteListWidget *const mNoteList;
};

class KNOTES_EXPORT KNoteFindDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KNoteFindDialog(QWidget *parent = nullptr);
    ~KNoteFindDialog() override;
    void setExistingNotes(const QHash<Akonadi::Item::Id, Akonadi::Item> &notes);

Q_SIGNALS:
    void noteSelected(Akonadi::Item::Id);

private:
    void writeConfig();
    void readConfig();
    KNoteFindWidget *const mNoteFindWidget;
};
