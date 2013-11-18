/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2013, The KNotes Developers

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

#ifndef KNOTEBUTTON_H
#define KNOTEBUTTON_H

#include <QPushButton>


class KNoteButton : public QPushButton
{
    Q_OBJECT
public:
    explicit KNoteButton( const QString &icon, QWidget *parent = 0 );
    ~KNoteButton();
    
    virtual int heightForWidth( int w ) const;
    virtual QSize sizeHint() const;
    
protected:
    void enterEvent( QEvent * );
    void leaveEvent( QEvent * );
    
    void paintEvent ( QPaintEvent *e );
};

#endif
