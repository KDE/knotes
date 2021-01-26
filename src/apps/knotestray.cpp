/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knotestray.h"

#include <KColorScheme>
#include <KIconLoader>
#include <KLocalizedString>

#include <QFontDatabase>
#include <QPainter>
#include <QWidget>

KNotesTray::KNotesTray(QWidget *parent)
    : KStatusNotifierItem(parent)
{
    setToolTipTitle(i18n("KNotes: Sticky notes by KDE"));
    setToolTipIconByName(QStringLiteral("knotes"));
    setStatus(KStatusNotifierItem::Active);
    setCategory(KStatusNotifierItem::ApplicationStatus);
    setStandardActionsEnabled(false);
    mIcon = QIcon::fromTheme(QStringLiteral("knotes"));
}

KNotesTray::~KNotesTray()
{
}

void KNotesTray::updateNumberOfNotes(int value)
{
    const int overlaySize = KIconLoader::SizeSmallMedium;

    const QString countString = QString::number(value);
    QFont countFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    countFont.setBold(true);

    // decrease the size of the font for the number of unread messages if the
    // number doesn't fit into the available space
    float countFontSize = countFont.pointSizeF();
    QFontMetrics qfm(countFont);
    const int width = qfm.width(countString);
    if (width > (overlaySize - 2)) {
        countFontSize *= float(overlaySize - 2) / float(width);
        countFont.setPointSizeF(countFontSize);
    }

    // Paint the number in a pixmap
    QPixmap overlayPixmap(overlaySize, overlaySize);
    overlayPixmap.fill(Qt::transparent);

    QPainter p(&overlayPixmap);
    p.setFont(countFont);
    KColorScheme scheme(QPalette::Active, KColorScheme::View);

    p.setBrush(Qt::NoBrush);
    p.setPen(scheme.foreground(KColorScheme::LinkText).color());
    p.setOpacity(1.0);
    p.drawText(overlayPixmap.rect(), Qt::AlignCenter, countString);
    p.end();

    QPixmap iconPixmap = mIcon.pixmap(overlaySize, overlaySize);

    QPainter pp(&iconPixmap);
    pp.drawPixmap(0, 0, overlayPixmap);
    pp.end();

    setIconByPixmap(iconPixmap);
}
