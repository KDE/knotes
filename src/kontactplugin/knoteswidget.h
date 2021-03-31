/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QWidget>
class KNotesIconView;
class KNotesPart;
class KNotesListWidgetSearchLine;
class KNotesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KNotesWidget(KNotesPart *part, QWidget *parent = nullptr);
    ~KNotesWidget() override;

    KNotesIconView *notesView() const;

    void updateClickMessage(const QString &shortcutStr);
public Q_SLOTS:
    void slotFocusQuickSearch();

private:
    KNotesIconView *mIconView = nullptr;
    KNotesListWidgetSearchLine *mSearchLine = nullptr;
};

