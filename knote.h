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

#ifndef KNOTE_H
#define KNOTE_H

#include <qstring.h>
#include <qframe.h>
#include <qpoint.h>
#include <qdir.h>

class QLabel;

class KPopupMenu;
class KNoteButton;
class KNoteEdit;


class KNote : public QFrame
{
    Q_OBJECT
public:
    KNote( const QString& config, bool load=false, QWidget* parent=0, const char* name=0 );
    ~KNote();

    void saveData() const;
    void saveConfig() const;
    void saveDisplayConfig() const;

    int noteId() const;
    QString name() const;
    QString text() const;

    void setName( const QString& name );
    void setText( const QString& text );

    void sync( const QString& app );
    bool isNew( const QString& app ) const;
    bool isModified( const QString& app ) const;

public slots:
    void slotNewNote();
    void slotRename();
    void slotClose();
    void slotKill();
    void slotMail() const;
    void slotPrint() const;
    void slotInsDate();
    void slotPreferences();

    void slotAlwaysOnTop();
    void slotToDesktop( int id );
    void slotPrepareDesktopMenu();

    virtual void setFocus();
    virtual void show();

signals:
    void sigKilled( const QString& );
    void sigRenamed( const QString&, const QString& );
    void sigNewNote();

protected:
    virtual void resizeEvent( QResizeEvent* );
    virtual void closeEvent( QCloseEvent* );

    bool eventFilter( QObject*, QEvent* );

private slots:
    void slotApplyConfig();

private:
    void convertOldConfig();

    QDir         m_noteDir;
    QString      m_configFile;

    QLabel*      m_label;
    KNoteButton* m_button;
    KNoteEdit*   m_editor;

    KPopupMenu*  m_menu;
    KPopupMenu*  m_desktop_menu;

    QPoint       m_pointerOffset;
    int          m_idAlwaysOnTop;
    bool         m_dragging;
    bool         m_killSelf;
};

#endif
