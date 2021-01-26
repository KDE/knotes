/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 2002-2013 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include "knotebutton.h"

#include <KIconLoader>

#include <QPainter>
#include <QStyleOption>

KNoteButton::KNoteButton(const QString &icon, QWidget *parent)
    : QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    setFlat(true);

    if (!icon.isEmpty()) {
        setIcon(KIconLoader::global()->loadIcon(icon, KIconLoader::Small, 10));
    }
}

KNoteButton::~KNoteButton()
{
}

void KNoteButton::enterEvent(QEvent *)
{
    setFlat(false);
    repaint();
}

void KNoteButton::leaveEvent(QEvent *)
{
    setFlat(true);
    repaint();
}

int KNoteButton::heightForWidth(int w) const
{
    return w;
}

QSize KNoteButton::sizeHint() const
{
    return QSize(QPushButton::sizeHint().height(), QPushButton::sizeHint().height());
}

void KNoteButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    // the button
    QStyleOption opt;
    opt.initFrom(this);

    if (isEnabled()) {
        opt.state |= QStyle::State_Enabled;
    }
    if (isDown()) {
        opt.state |= QStyle::State_DownArrow;
    }
    if (isCheckable() && isChecked()) {
        opt.state |= QStyle::State_On;
    }
    if (!isFlat()) {
        if (!isDown()) {
            opt.state |= QStyle::State_Raised;
        } else {
            opt.state |= QStyle::State_MouseOver;
        }
    }

    if ((opt.state & QStyle::State_MouseOver) && (!(opt.state & QStyle::State_DownArrow))) {
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
    }

    // the button label
    if (!icon().isNull()) {
        QIcon::Mode mode = QIcon::Disabled;
        QIcon::State state = QIcon::Off;

        if (opt.state & QStyle::State_Enabled) {
            mode = hasFocus() ? QIcon::Active : QIcon::Normal;
        }
        if (opt.state & QStyle::State_On) {
            state = QIcon::On;
        }

        const QPixmap pix = icon().pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize), mode, state);

        int dx = (width() - pix.width()) / 2;
        int dy = (height() - pix.height()) / 2;

        // Shift button contents if pushed.
        if ((opt.state & QStyle::State_On) || (opt.state & QStyle::State_DownArrow)) {
            dx += style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal);
            dy += style()->pixelMetric(QStyle::PM_ButtonShiftVertical);
        }

        p.drawPixmap(dx, dy, pix);
    }
}
