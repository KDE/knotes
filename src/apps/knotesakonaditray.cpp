/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knotesakonaditray.h"
#include "knotesglobalconfig.h"
#include <AkonadiCore/ChangeRecorder>

#include <KColorScheme>
#include <KIconLoader>
#include <KLocalizedString>

#include <QFontDatabase>
#include <QPainter>
#include <QWidget>

KNotesAkonadiTray::KNotesAkonadiTray(QWidget *parent)
    : KStatusNotifierItem(parent)
    , mIcon(QIcon::fromTheme(QStringLiteral("knotes")))
{
    setToolTipTitle(i18n("KNotes: Sticky notes by KDE"));
    setToolTipIconByName(QStringLiteral("knotes"));
    setStatus(KStatusNotifierItem::Active);
    setCategory(KStatusNotifierItem::ApplicationStatus);
    setStandardActionsEnabled(false);
    // Initialize
    updateNumberOfNotes(0);
}

KNotesAkonadiTray::~KNotesAkonadiTray()
{
}

void KNotesAkonadiTray::slotGeneralPaletteChanged()
{
    const KColorScheme scheme(QPalette::Active, KColorScheme::View);
    mTextColor = scheme.foreground(KColorScheme::LinkText).color();
}

void KNotesAkonadiTray::updateNumberOfNotes(int number)
{
    if (KNotesGlobalConfig::self()->systemTrayShowNotes() && number != 0) {
        const int overlaySize = KIconLoader::SizeSmallMedium;

        const QString countString = QString::number(number);
        QFont countFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        countFont.setBold(true);

        // decrease the size of the font for the number of unread messages if the
        // number doesn't fit into the available space
        float countFontSize = countFont.pointSizeF();
        QFontMetrics qfm(countFont);
        const int width = qfm.boundingRect(countString).width();
        if (width > (overlaySize - 2)) {
            countFontSize *= float(overlaySize - 2) / float(width);
            countFont.setPointSizeF(countFontSize);
        }

        // Paint the number in a pixmap
        QPixmap overlayPixmap(overlaySize, overlaySize);
        overlayPixmap.fill(Qt::transparent);

        QPainter p(&overlayPixmap);
        p.setFont(countFont);
        if (!mTextColor.isValid()) {
            slotGeneralPaletteChanged();
        }

        p.setBrush(Qt::NoBrush);
        p.setPen(mTextColor);
        p.setOpacity(1.0);
        p.drawText(overlayPixmap.rect(), Qt::AlignCenter, countString);
        p.end();

        QPixmap iconPixmap = mIcon.pixmap(overlaySize, overlaySize);

        QPainter pp(&iconPixmap);
        pp.drawPixmap(0, 0, overlayPixmap);
        pp.end();

        setIconByPixmap(iconPixmap);
    } else {
        setIconByPixmap(mIcon);
    }
}
