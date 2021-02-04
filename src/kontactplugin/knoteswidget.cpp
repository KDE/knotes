/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knoteswidget.h"
#include "knotesiconview.h"
#include "knoteslistwidgetsearchline.h"

#include <QVBoxLayout>

KNotesWidget::KNotesWidget(KNotesPart *part, QWidget *parent)
    : QWidget(parent)
{
    auto lay = new QVBoxLayout(this);
    mSearchLine = new KNotesListWidgetSearchLine;
    lay->addWidget(mSearchLine);
    mIconView = new KNotesIconView(part, parent);
    mSearchLine->setListWidget(mIconView);
    lay->addWidget(mIconView);
}

KNotesWidget::~KNotesWidget()
{
}

void KNotesWidget::slotFocusQuickSearch()
{
    mSearchLine->setFocus();
}

void KNotesWidget::updateClickMessage(const QString &shortcutStr)
{
    mSearchLine->updateClickMessage(shortcutStr);
}

KNotesIconView *KNotesWidget::notesView() const
{
    return mIconView;
}
