/*
 *   knotes - Notes for the KDE project
 *   This file only: Copyright (C) 1999  Espen Sand, espen@kde.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _OPTION_DIALOG_H_
#define _OPTION_DIALOG_H_ 

class QCheckBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class KColorButton;
class KFontChooser;

#include <kdialogbase.h>

#include "knotes.h"

class OptionDialog : public KDialogBase
{
  Q_OBJECT
  
  public:
    OptionDialog( QWidget *parent=0, const char *name=0, bool modal=true );
    ~OptionDialog( void );

    void setState( const DefStruct &state );
    DefStruct state( void );
    
  protected slots:
    virtual void slotOk( void );
    virtual void slotApply( void ); 
    virtual void slotCancel( void ); 

  private:
    void setupSettingPage( void );
    void setupFontPage( void );
    void setupAboutPage( void );

  private:
    KColorButton *mFgColor;
    KColorButton *mBgColor;
    QSpinBox     *mWidthSpin;
    QSpinBox     *mHeightSpin;
    QCheckBox    *m3dCheck;
    QCheckBox    *mAutoIndentCheck;
    QCheckBox    *mSoundCheck;
    QLineEdit    *mPrintEdit;
    QLineEdit    *mMailEdit;
    QLineEdit    *mSoundEdit;

    KFontChooser *mFontChooser;

    DefStruct    mState;

  signals:
    void valueChanged( const DefStruct &state );
};

#endif
