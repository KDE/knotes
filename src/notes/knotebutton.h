/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2013 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#pragma once

#include <QPushButton>

class KNoteButton : public QPushButton
{
    Q_OBJECT
public:
    explicit KNoteButton(const QString &icon, QWidget *parent = nullptr);
    ~KNoteButton() override;

    Q_REQUIRED_RESULT int heightForWidth(int w) const override;
    Q_REQUIRED_RESULT QSize sizeHint() const override;

protected:
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;

    void paintEvent(QPaintEvent *e) override;
};

