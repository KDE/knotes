/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2001, The KNotes Developers

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
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*******************************************************************/

#ifndef KNOTEBUTTON_H
#define KNOTEBUTTON_H

#include <qpushbutton.h>
#include <qdrawutil.h>


class KNoteButton: public QPushButton
{
    Q_OBJECT
public:
    KNoteButton( QWidget* parent=0, const char* name=0 )
        : QPushButton( parent, name )
    {
        setFocusPolicy( NoFocus );
        flat = true;
        last_button = 0;
    }
    ~KNoteButton() {}

private:
    bool flat;
    int  last_button;

protected:
    void enterEvent( QEvent * )
    {
        flat = false;
        repaint( false );
    }

    void leaveEvent( QEvent * )
    {
        flat = true;
        repaint();
    }

    void mousePressEvent( QMouseEvent *e )
    {
        if ( isDown() )
            return;

        bool hit = hitButton( e->pos() );
        if ( hit )
        {
            last_button = e->button();
            setDown( TRUE );
            repaint( FALSE );
            emit pressed();
        }
    }

    void mouseReleaseEvent( QMouseEvent *e )
    {
        if ( !isDown() )
        {
            last_button = 0;
            return;
        }

        bool hit = hitButton( e->pos() );
        setDown( FALSE );
        if ( hit )
        {
            if ( isToggleButton() )
                setOn( !isOn() );
            repaint( FALSE );

            if ( isToggleButton() )
                emit toggled( isOn() );
            emit released();
            emit clicked();
        }
        else
        {
            repaint();
            emit released();
        }
        last_button = 0;
    }
    
    void mouseMoveEvent( QMouseEvent *e )
    {
        if ( !last_button )
            return;

        if ( !(e->state() & LeftButton) &&
             !(e->state() & MidButton) &&
             !(e->state() & RightButton))
            return;

        bool hit = hitButton( e->pos() );
        if ( hit )
        {
            if ( !isDown() )
            {
                setDown( true );
                repaint( false );
                emit pressed();
            }
        }
        else
        {
            if ( isDown() )
            {
                setDown( false );
                repaint();
                emit released();
            }
        }
    }

    void paint( QPainter* painter )
    {
        if ( !painter )
            return;

        if ( isDown() || (isOn() && !flat) )
        {
            if ( style().guiStyle() == WindowsStyle )
                qDrawWinButton( painter, 0, 0, width(),
                                height(), colorGroup(), TRUE );
            else
                qDrawShadePanel( painter, 0, 0, width(),
                                 height(), colorGroup(), TRUE, 2, 0L );
        }
        else if ( !flat )
        {
            if ( style().guiStyle() == WindowsStyle )
                qDrawWinButton( painter, 0, 0, width(), height(),
                                colorGroup(), FALSE );
            else
                qDrawShadePanel( painter, 0, 0, width(), height(),
                                 colorGroup(), FALSE, 2, 0L );
        }

        int dx = ( width() - pixmap()->width() ) / 2;
        int dy = ( height() - pixmap()->height() ) / 2;
        if ( isDown() && style().guiStyle() == WindowsStyle )
        {
            dx++;
            dy++;
        }
        painter->drawPixmap( dx, dy, *pixmap() );
    }

    void drawButton( QPainter* p )
    {
        paint( p );
    }

    void drawButtonLabel( QPainter* p )
    {
        paint( p );
    }
};

#endif
