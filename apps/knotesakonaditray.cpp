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

#include "knotesakonaditray.h"
#include <AkonadiCore/ChangeRecorder>
#include "knotesglobalconfig.h"

#include <KIconLoader>
#include <KGlobalSettings>
#include <KLocalizedString>
#include <KColorScheme>

#include <QPainter>
#include <QWidget>
#include <QFontDatabase>

KNotesAkonadiTray::KNotesAkonadiTray(QWidget *parent)
    : KStatusNotifierItem(parent),
      mIcon( QLatin1String("knotes") )
{
    setToolTipTitle( i18n( "KNotes: Sticky notes for KDE" ) );
    setToolTipIconByName( QLatin1String("knotes") );
    setStatus( KStatusNotifierItem::Active );
    setCategory( KStatusNotifierItem::ApplicationStatus );
    setStandardActionsEnabled(false);
    //Initialize
    updateNumberOfNotes(0);
    connect(KGlobalSettings::self(), &KGlobalSettings::kdisplayPaletteChanged, this, &KNotesAkonadiTray::slotGeneralPaletteChanged);
}

KNotesAkonadiTray::~KNotesAkonadiTray()
{
}

void KNotesAkonadiTray::slotGeneralPaletteChanged()
{
    const KColorScheme scheme( QPalette::Active, KColorScheme::View );
    mTextColor = scheme.foreground( KColorScheme::LinkText ).color();
}

void KNotesAkonadiTray::updateNumberOfNotes(int number)
{
    if (KNotesGlobalConfig::self()->systemTrayShowNotes() && number != 0) {
        const int overlaySize = KIconLoader::SizeSmallMedium;

        const QString countString = QString::number( number );
        QFont countFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        countFont.setBold(true);

        // decrease the size of the font for the number of unread messages if the
        // number doesn't fit into the available space
        float countFontSize = countFont.pointSizeF();
        QFontMetrics qfm( countFont );
        const int width = qfm.width( countString );
        if ( width > (overlaySize - 2) ) {
            countFontSize *= float( overlaySize - 2 ) / float( width );
            countFont.setPointSizeF( countFontSize );
        }

        // Paint the number in a pixmap
        QPixmap overlayPixmap( overlaySize, overlaySize );
        overlayPixmap.fill( Qt::transparent );

        QPainter p( &overlayPixmap );
        p.setFont( countFont );
        if (!mTextColor.isValid()) {
            slotGeneralPaletteChanged();
        }

        p.setBrush( Qt::NoBrush );
        p.setPen(mTextColor);
        p.setOpacity( 1.0 );
        p.drawText( overlayPixmap.rect(),Qt::AlignCenter, countString );
        p.end();

        QPixmap iconPixmap = mIcon.pixmap(overlaySize, overlaySize);

        QPainter pp(&iconPixmap);
        pp.drawPixmap(0, 0, overlayPixmap);
        pp.end();

        setIconByPixmap( iconPixmap );
    } else {
        setIconByPixmap(mIcon);
    }
}

