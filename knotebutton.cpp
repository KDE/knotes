/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 2002, 2003, The KNotes Developers

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
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*******************************************************************/

#include <qstyle.h>
#include <qpainter.h>
#include <qiconset.h>
#include <qsizepolicy.h>

#include <kglobal.h>
#include <kicontheme.h>
#include <kiconloader.h>

#include "knotebutton.h"


KNoteButton::KNoteButton( const QString& icon, QWidget *parent, const char *name )
    : QPushButton( parent, name )
{
    setFocusPolicy( NoFocus );
    setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
    
    m_flat = true;

    if ( !icon.isEmpty() )
        setIconSet( KGlobal::iconLoader()->loadIconSet( icon, KIcon::Small, 10 ) );
}

KNoteButton::~KNoteButton() 
{
}

void KNoteButton::enterEvent( QEvent * )
{
    m_flat = false;
    repaint( false );
}

void KNoteButton::leaveEvent( QEvent * )
{
    m_flat = true;
    repaint();
}

QSize KNoteButton::sizeHint() const
{
    return QSize( QPushButton::sizeHint().height(), QPushButton::sizeHint().height() );
}

void KNoteButton::drawButton( QPainter* p )
{
    QStyle::SFlags flags = QStyle::Style_Default;

    if ( isEnabled() )
        flags |= QStyle::Style_Enabled;
    if ( isDown() )
        flags |= QStyle::Style_Down;
    if ( isOn() )
        flags |= QStyle::Style_On;
    if ( !isFlat() && !isDown() )
        flags |= QStyle::Style_Raised;
    if ( !m_flat )
        flags |= QStyle::Style_MouseOver;

    style().drawPrimitive( QStyle::PE_ButtonTool, p, rect(), colorGroup(), flags );
    drawButtonLabel( p );
}

void KNoteButton::drawButtonLabel( QPainter* p )
{
    if ( iconSet() && !iconSet()->isNull() )
    {
        QIconSet::Mode  mode  = QIconSet::Disabled;
        QIconSet::State state = QIconSet::Off;

        if ( isEnabled() )
            mode = hasFocus() ? QIconSet::Active : QIconSet::Normal;
        if ( isToggleButton() && isOn() )
            state = QIconSet::On;

        QPixmap pix = iconSet()->pixmap( QIconSet::Small, mode, state );

        int dx = ( width() - pix.width() ) / 2;
        int dy = ( height() - pix.height() ) / 2;

        // Shift button contents if pushed.
        if ( isOn() || isDown() )
        {
            dx += style().pixelMetric( QStyle::PM_ButtonShiftHorizontal, this );
            dy += style().pixelMetric( QStyle::PM_ButtonShiftVertical, this );
        }
        
        p->drawPixmap( dx, dy, pix );
    }
}
