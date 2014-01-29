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

#include "knotestray.h"

#include <KIconLoader>
#include <KGlobalSettings>
#include <KLocalizedString>
#include <KColorScheme>

#include <QPainter>
#include <QWidget>

KNotesTray::KNotesTray(QWidget *parent)
    : KStatusNotifierItem(parent)
{
    setToolTipTitle( i18n( "KNotes: Sticky notes for KDE" ) );
    setToolTipIconByName( QLatin1String("knotes") );
    setStatus( KStatusNotifierItem::Active );
    setCategory( KStatusNotifierItem::ApplicationStatus );
    setStandardActionsEnabled(false);
    mIcon = KIcon( QLatin1String("knotes") );
}

KNotesTray::~KNotesTray()
{
}

void KNotesTray::updateNumberOfNotes(int value)
{
    const int overlaySize = KIconLoader::SizeSmallMedium;

    const QString countString = QString::number( value );
    QFont countFont = KGlobalSettings::generalFont();
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
    KColorScheme scheme( QPalette::Active, KColorScheme::View );

    p.setBrush( Qt::NoBrush );
    p.setPen( scheme.foreground( KColorScheme::LinkText ).color() );
    p.setOpacity( 1.0 );
    p.drawText( overlayPixmap.rect(),Qt::AlignCenter, countString );
    p.end();

    QPixmap iconPixmap = mIcon.pixmap(overlaySize, overlaySize);

    QPainter pp(&iconPixmap);
    pp.drawPixmap(0, 0, overlayPixmap);
    pp.end();

    setIconByPixmap( iconPixmap );
}


#include "moc_knotestray.cpp"
