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

#ifndef KNOTECONFIGDLG_H
#define KNOTECONFIGDLG_H

#include <kdialogbase.h>
#include <kconfig.h>
#include <klineedit.h>
#include <kcolorbtn.h>
#include <kcolordialog.h>
#include <knuminput.h>

#include <qcheckbox.h>
#include <qfont.h>

class KNoteConfigDlg : public KDialogBase
{
   Q_OBJECT
public:
    KNoteConfigDlg( const QString& configfile, const QString & title,
                    QWidget* parent=0, const char* name=0 );
    ~KNoteConfigDlg();

    void makeDisplayPage();
    void makeEditorPage();
    void makeActionsPage();

protected:
    KConfig*      _config;

    KColorButton* _fgColor;
    KColorButton* _bgColor;

    KIntNumInput* _widthEdit;
    KIntNumInput* _heightEdit;
    KIntNumInput* _tabEdit;

    QCheckBox*    _autoIndentSwitch;
    KLineEdit*    _mailEdit;
    KLineEdit*    _printEdit;
    QPushButton*  _font;

protected slots:
    virtual void slotOk();
    virtual void slotApply();

    void slotChangeFont();
    void slotFGColor( const QColor& );
    void slotBGColor( const QColor& );

signals:
    void updateConfig();

private:
    void storeSettings();

};

#endif
