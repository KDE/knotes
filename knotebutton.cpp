/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 2002-2006, The KNotes Developers

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*******************************************************************/

#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QIcon>
#include <QSizePolicy>
#include <QPixmap>

#include <kglobal.h>
#include <kicontheme.h>
#include <kiconloader.h>

#include "knotebutton.h"


KNoteButton::KNoteButton( const QString& icon, QWidget *parent )
    : QPushButton( parent )
{
    setFocusPolicy( Qt::NoFocus );
    setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );

    setFlat( true );

    if ( !icon.isEmpty() )
        setIcon( KGlobal::iconLoader()->loadIconSet( icon, KIcon::Small, 10 ) );
}

KNoteButton::~KNoteButton()
{
}

void KNoteButton::enterEvent( QEvent * )
{
    setFlat( false );
    repaint();
}

void KNoteButton::leaveEvent( QEvent * )
{
    setFlat( true );
    repaint();
}

int KNoteButton::heightForWidth( int w ) const
{
    return w;
}

QSize KNoteButton::sizeHint() const
{
    return QSize( QPushButton::sizeHint().height(), QPushButton::sizeHint().height() );
}

void KNoteButton::paintEvent( QPaintEvent * )
{
    QPainter p( this );

    // the button
    QStyleOption opt;
    opt.initFrom( this );

    if ( isEnabled() )
        opt.state |= QStyle::State_Enabled;
    if ( isDown() )
        opt.state |= QStyle::State_DownArrow;
    if ( isChecked() )
        opt.state |= QStyle::State_On;
    if ( !isFlat() )
        if ( !isDown() )
            opt.state |= QStyle::State_Raised;
        else
            opt.state |= QStyle::State_MouseOver;

    style()->drawPrimitive( QStyle::PE_PanelButtonTool, &opt, &p, this );

    // the button label
    if ( icon().isNull() )
    {
        QIcon::Mode  mode  = QIcon::Disabled;
        QIcon::State state = QIcon::Off;

        if ( isEnabled() )
            mode = hasFocus() ? QIcon::Active : QIcon::Normal;
        if ( isCheckable() && isChecked() )
            state = QIcon::On;

        QPixmap pix = icon().pixmap( style()->pixelMetric( QStyle::PM_SmallIconSize ), mode, state );

        int dx = ( width() - pix.width() ) / 2;
        int dy = ( height() - pix.height() ) / 2;

        // Shift button contents if pushed.
        if ( isChecked() || isDown() )
        {
            dx += style()->pixelMetric( QStyle::PM_ButtonShiftHorizontal );
            dy += style()->pixelMetric( QStyle::PM_ButtonShiftVertical );
        }

        p.drawPixmap( dx, dy, pix );
    }
}
