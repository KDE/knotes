/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2006, The KNotes Developers

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

#ifndef KNOTE_H
#define KNOTE_H

#include <QDomDocument>
#include <QEvent>
#include <QFrame>

#include <kconfig.h>
#include <kxmlguiclient.h>
#include <KSharedConfig>

class QLabel;
class QLayout;
class QSizeGrip;

class KFind;
class KMenu;
class KNoteButton;
class KNoteConfig;
class KNoteEdit;
class KSelectAction;
class KToggleAction;
class KToolBar;

namespace KCal {
class Journal;
}


class KNote
  : public QFrame, virtual public KXMLGUIClient
{
  Q_OBJECT
  public:
    KNote( const QDomDocument& buildDoc, KCal::Journal *journal, QWidget *parent = 0 );
    ~KNote();

    void changeJournal(KCal::Journal *);
    void saveData( bool update = true);
    void saveConfig() const;

    QString noteId() const;
    QString name() const;
    QString text() const;

    void setName( const QString &name );
    void setText( const QString &text );

    void find( KFind* kfind );

    bool isModified() const;
    bool isDesktopAssigned() const;
    void blockEmitDataChanged( bool _b ) { m_blockEmitDataChanged = _b;}
    void commitData();

    void toDesktop( int desktop );

  public slots:
    void slotRename();
    void slotKill( bool force = false );
    void slotClose();

  signals:
    void sigRequestNewNote();
    void sigShowNextNote();
    void sigNameChanged(const QString &);
    void sigDataChanged(const QString &);
    void sigColorChanged();
    void sigKillNote( KCal::Journal * );

    void sigFindFinished();

  protected:
    virtual void contextMenuEvent( QContextMenuEvent * );
    virtual void showEvent( QShowEvent * );
    virtual void resizeEvent( QResizeEvent * );
    virtual void closeEvent( QCloseEvent * );
    virtual void dropEvent( QDropEvent * );
    virtual void dragEnterEvent( QDragEnterEvent * );

    virtual bool event( QEvent * );
    virtual bool eventFilter( QObject *, QEvent * );

  private slots:
    void slotUpdateReadOnly();

    void slotSend();
    void slotMail();
    void slotPrint();
    void slotSaveAs();

    void slotInsDate();
    void slotSetAlarm();

    void slotPreferences();
    void slotPopupActionToDesktop( int id );

    void slotFindNext();
    void slotHighlight( const QString &txt, int idx, int len );

    void slotApplyConfig();
    void slotUpdateKeepAboveBelow();
    void slotUpdateShowInTaskbar();
    void slotUpdateDesktopActions();

    void slotKeepAbove();
    void slotKeepBelow();

    void slotRequestNewNote();

  private:
    void buildGui();
    void createActions();
    void createNoteEditor();
    void createNoteFooter();
    void createNoteHeader();
    void prepare();

    void updateFocus();
    void updateLayout();
    void updateLabelAlignment();

    void setColor( const QColor &, const QColor & );

  private:
    QLayout       *m_noteLayout;
    QLabel        *m_label;
    QSizeGrip     *m_grip;
    KNoteButton   *m_button;
    KToolBar      *m_tool;
    KNoteEdit     *m_editor;

    KNoteConfig   *m_config;
    KCal::Journal *m_journal;

    KFind         *m_find;
    KMenu         *m_menu;

    KToggleAction *m_readOnly;

#ifdef Q_WS_X11
    KSelectAction   *m_toDesktop;
#endif
    KToggleAction *m_keepAbove;
    KToggleAction *m_keepBelow;

    KSharedConfig::Ptr m_kwinConf;
    bool m_blockEmitDataChanged;
    bool mBlockWriteConfigDuringCommitData;
};

#endif
