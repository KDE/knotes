/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2003, The KNotes Developers

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

#ifndef KNOTECONFIGDLG_H
#define KNOTECONFIGDLG_H

#include <kdialogbase.h>

class QString;
class QColor;
class QCheckBox;
class QPushButton;

class KConfig;
class KColorButton;
class KLineEdit;
class KIntNumInput;


class KNoteConfigDlg : public KDialogBase
{
    Q_OBJECT
public:
    KNoteConfigDlg( const QString &configfile, const QString &title,
                    bool global, QWidget *parent=0, const char *name=0 );
    ~KNoteConfigDlg();

    void makeDisplayPage();
    void makeEditorPage();
    void makeActionsPage();

protected slots:
    virtual void slotOk();
    virtual void slotApply();

    void slotChangeTitleFont();
    void slotChangeTextFont();
    void slotFGColor( const QColor& );
    void slotBGColor( const QColor& );

signals:
    void updateConfig();
    void skipTaskbar( bool skip );

private:
    void storeSettings();

    bool m_global;

    KConfig *m_config;

    KColorButton *m_fgColor;
    KColorButton *m_bgColor;
    KIntNumInput *m_widthEdit;
    KIntNumInput *m_heightEdit;
    QCheckBox    *m_skipTaskbarSwitch;
    
    KIntNumInput *m_tabEdit;
    QCheckBox    *m_autoIndentSwitch;
    QCheckBox    *m_richTextSwitch;
    QPushButton  *m_titleFont;
    QPushButton  *m_textFont;

    KLineEdit    *m_mailEdit;
};

#endif
