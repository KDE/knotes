/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2004, The KNotes Developers

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

#ifndef KNOTE_H
#define KNOTE_H

#include <qstring.h>
#include <qevent.h>
#include <qframe.h>
#include <qpoint.h>
#include <qcolor.h>

#include <kxmlguiclient.h>

class QLabel;

class KXMLGUIBuilder;

class KPopupMenu;
class KNoteButton;
class KNoteEdit;
class KToolBar;
class KListAction;
class KToggleAction;

namespace KCal {
    class Journal;
}


class KNote : public QFrame, virtual public KXMLGUIClient
{
    Q_OBJECT
public:
    KNote( KXMLGUIBuilder *builder, QDomDocument buildDoc, KCal::Journal *journal,
           QWidget *parent=0, const char *name=0 );
    ~KNote();

    void saveData();
    void saveConfig() const;

    QString noteId() const;
    QString name() const;
    QString text() const;

    void setName( const QString& name );
    void setText( const QString& text );

    void sync( const QString& app );
    bool isNew( const QString& app ) const;
    bool isModified( const QString& app ) const;
    void toDesktop( int desktop );

public slots:
    void slotRename();
    void slotClose();
    void slotKill( bool force = false );
    void slotMail() /*const*/;
    void slotPrint();
    void slotInsDate();
    void slotPreferences();

    void slotToggleAlwaysOnTop();
    void slotPopupActionToDesktop( int id );
    void slotUpdateDesktopActions();

signals:
    void sigRequestNewNote();
    void sigNameChanged();
    void sigDataChanged();
    void sigKillNote( KCal::Journal* );

protected:
    virtual void showEvent( QShowEvent* );
    virtual void resizeEvent( QResizeEvent* );
    virtual void closeEvent( QCloseEvent* );
    virtual void keyPressEvent( QKeyEvent* );

    virtual bool event( QEvent* );
    virtual bool eventFilter( QObject*, QEvent* );

private slots:
    void slotApplyConfig();
    void slotSkipTaskbar( bool skip );

private:
    void updateFocus();
    void updateLayout();
    void updateLabelAlignment();
    void setColor( const QColor&, const QColor& );

private:
    QPoint m_pointerOffset;
    bool   m_dragging;

    QLabel      *m_label;
    KNoteButton *m_button;
    KToolBar    *m_tool;
    KNoteEdit   *m_editor;

    KCal::Journal *m_journal;
    QString        m_configFile;

    KToggleAction *m_alwaysOnTop;
    KListAction   *m_toDesktop;

    KPopupMenu *m_menu;
    KPopupMenu *m_edit_menu;
};

#endif
