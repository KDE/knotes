/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (C) Bernd Johannes Wuebben
     wuebben@math.cornell.edu
     wuebben@kde.org

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

#ifndef KNOTE_H
#define KNOTE_H

#include "knoteedit.h"
#include "knotebutton.h"

#include <qstring.h>
#include <qframe.h>
#include <qpoint.h>


class QLabel;

class KPopupMenu;

class KNote : public QFrame  {
   Q_OBJECT
public:
    KNote( QString configfile, bool oldconfig=false, QWidget* parent=0, const char* name=0 );
    ~KNote();

    void saveData();
    void saveConfig();
    void saveDisplayConfig();
    QString getName();

    void setText( const QString& text );

public slots:
    //menu slots
    void slotMail   ( int );
    void slotPrint  ( int );
    void slotRename ( int );
    void slotInsDate( int );
    void slotKill   ( int );
    void slotClose  ();
    void slotNewNote( int );
    void slotPreferences( int );

    void slotApplyConfig();
    void slotToDesktop( int id );
    void slotPrepareDesktopMenu();
    void slotAlwaysOnTop( int );

    virtual void setFocus();
    virtual void show();

signals:
    void sigKilled( QString );
    void sigRenamed( QString&, QString& );
    void sigNewNote( int );

protected:
    virtual void resizeEvent( QResizeEvent* );
    virtual void closeEvent( QCloseEvent* e );

    bool eventFilter( QObject* o, QEvent* e );

private:
    void           convertOldConfig();

    QString        m_configfile;

    KNoteEdit*     m_editor;
    KNoteButton*   m_button;
    QLabel*        m_label;
    KPopupMenu*    m_menu;
    KPopupMenu*    m_desktop_menu;

    bool           m_dragging;
    QPoint         m_pointerOffset;
    int            m_idAlwaysOnTop;
    bool           m_saveself;
};


#endif
