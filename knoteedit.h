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

#ifndef KNOTEEDIT_H
#define KNOTEEDIT_H

#include <qwidget.h>
#include <qtextedit.h>

using namespace Qt3;

class KNoteEdit : public QTextEdit
{
    Q_OBJECT
public:
    KNoteEdit( QWidget *parent=0, const char *name=0 );
    ~KNoteEdit();

    void setAutoIndentMode( bool newmode );
    void readFile( QString& filename );
    void dumpToFile( QString& filename ) const;

protected:
    void dragMoveEvent( QDragMoveEvent* event );
    void dragEnterEvent( QDragEnterEvent* event );
    void dropEvent( QDropEvent* event );
/*
    void mouseDoubleClickEvent( QMouseEvent* e );
    void keyPressEvent( QKeyEvent* e );
    void mynewLine();
*/
protected slots:
    void slotReturnPressed();
private:
    QString prefixString( QString string );
    bool m_autoIndentMode;

signals:
    void gotUrlDrop( const QString& url );
};

#endif
