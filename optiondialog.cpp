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

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qspinbox.h>

#include <kcolorbtn.h>
#include <kfontdialog.h>
#include <kiconloader.h>
#include <klocale.h>

#include "optiondialog.h"
#include "version.h"


OptionDialog::OptionDialog( QWidget *parent, char *name, bool modal )
  :KDialogBase( Tabbed, i18n("Configuration"), Help|Apply|Ok|Cancel,
		Ok, parent, name, modal )
{
  setHelp( "knotes/index.html", QString::null );
 
  setupSettingPage();
  setupFontPage();
  setupAboutPage();
}

OptionDialog::~OptionDialog( void )
{
}


void OptionDialog::setState( const DefStruct &state )
{
  mState = state;

  mFgColor->setColor( mState.forecolor );
  mBgColor->setColor( mState.backcolor );
  mWidthSpin->setValue( mState.width );
  mHeightSpin->setValue( mState.height );
  m3dCheck->setChecked( mState.frame3d );
  mAutoIndentCheck->setChecked( mState.autoindent );
  mSoundCheck->setChecked( mState.playSound );
  mPrintEdit->setText( mState.mailcommand );
  mMailEdit->setText( mState.mailcommand );
  mSoundEdit->setText( mState.soundcommand );
  mFontChooser->setFont( mState.font );
}


DefStruct OptionDialog::state( void )
{
  DefStruct state;

  state.forecolor    = mFgColor->color();
  state.backcolor    = mBgColor->color();
  state.width        = mWidthSpin->value();
  state.height       = mHeightSpin->value();
  state.frame3d      = m3dCheck->isChecked();
  state.autoindent   = mAutoIndentCheck->isChecked();
  state.playSound    = mSoundCheck->isChecked();
  state.mailcommand  = mPrintEdit->text();
  state.mailcommand  = mMailEdit->text();
  state.soundcommand = mSoundEdit->text();
  state.font         = mFontChooser->font();

  return( state );
}


void OptionDialog::slotOk( void )  
{
  slotApply();
  accept();
}


void OptionDialog::slotApply( void )  
{
  mState = state();
  emit valueChanged( mState );
}


void OptionDialog::slotCancel( void )  
{
  setState( mState );
  emit valueChanged( mState );
  reject();
}


void OptionDialog::setupSettingPage( void )
{
  QFrame *page = addPage( i18n("&Settings") );
  if( page == 0 ) { return; }
  
  QGridLayout *topLayout = new QGridLayout( page, 10, 3, 0, spacingHint() );
  
  QLabel *label = new QLabel( i18n("Text color:"), page );
  topLayout->addWidget( label, 0, 0 );
  label = new QLabel( i18n("Background color:"), page );
  topLayout->addWidget( label, 1, 0 );

  mFgColor = new KColorButton( page );
  mFgColor->setFixedWidth( fontMetrics().maxWidth() * 8 );
  topLayout->addWidget( mFgColor, 0, 1, AlignLeft );
  mBgColor = new KColorButton( page );
  mBgColor->setFixedWidth( fontMetrics().maxWidth() * 8 );
  topLayout->addWidget( mBgColor, 1, 1, AlignLeft );

  topLayout->addRowSpacing( 2, spacingHint()*2 );

  label = new QLabel( i18n("Width [pixels]:"), page );
  topLayout->addWidget( label, 3, 0 );
  label = new QLabel( i18n("Height [pixels]:"), page );
  topLayout->addWidget( label, 4, 0 );
  mWidthSpin = new QSpinBox( 0, 65535, 1, page );
  mWidthSpin->setFixedWidth( fontMetrics().maxWidth()*6 );
  topLayout->addWidget( mWidthSpin, 3, 1, AlignLeft );
  mHeightSpin = new QSpinBox( 0, 65535, 1, page );
  mHeightSpin->setFixedWidth( fontMetrics().maxWidth()*6 );
  topLayout->addWidget( mHeightSpin, 4, 1, AlignLeft );

  m3dCheck = new QCheckBox( page );
  m3dCheck->setText(i18n("3d Frame") );
  topLayout->addWidget( m3dCheck, 3, 2 );
  mAutoIndentCheck = new QCheckBox( page );
  mAutoIndentCheck->setText(i18n("Auto Indent") );
  topLayout->addWidget( mAutoIndentCheck, 4, 2 );

  topLayout->addRowSpacing( 5, spacingHint()*2 );

  label = new QLabel( i18n("Print Command:"), page );
  topLayout->addWidget( label, 6, 0 );
  mPrintEdit = new QLineEdit( page );
  topLayout->addMultiCellWidget( mPrintEdit, 6, 6, 1, 2 );

  label = new QLabel( i18n("Mail Command:"), page );
  topLayout->addWidget( label, 7, 0 );
  mMailEdit = new QLineEdit( page );
  topLayout->addMultiCellWidget( mMailEdit, 7, 7, 1, 2 );

  mSoundCheck = new QCheckBox( page );
  mSoundCheck->setText(i18n("Play sound:") );
  topLayout->addWidget( mSoundCheck, 8, 0 );
  mSoundEdit = new QLineEdit( page );
  topLayout->addMultiCellWidget( mSoundEdit, 8, 8, 1, 2 );

  topLayout->setRowStretch( 9, 10 );
}


void OptionDialog::setupFontPage( void )
{
  QFrame *page = addPage( i18n("&Font") );
  if( page == 0 ) { return; }

  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
  if( topLayout == 0 ) { return; }

  mFontChooser = new KFontChooser(page,"fonts",false,QStringList(),false,6);
  topLayout->addWidget( mFontChooser );
}



void OptionDialog::setupAboutPage( void )
{
  QFrame *page = addPage( i18n("About &KNotes") );
  if( page == 0 ) { return; }

  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
  if( topLayout == 0 ) { return; }

  QString authorText = i18n(""
    "KNotes %1\n"
    "Bernd Johannes Wuebben\n"
    "wuebben@math.cornell.edu\n"
    "wuebben@kde.org\n"
    "Copyright (C) 1997\n\n"
    "With contributions by:\n"
    "Matthias Ettrich <ettrich@kde.org>").arg(KNOTES_VERSION);

  topLayout->addSpacing( spacingHint() );

  QHBoxLayout *hlay = new QHBoxLayout( topLayout );
  hlay->addSpacing( spacingHint() );

  QLabel *logo = new QLabel(page);
  logo->setPixmap( BarIcon("knoteslogo") );
  hlay->addWidget( logo );

  QLabel *label = new QLabel( authorText, page );
  hlay->addWidget( label, 10, AlignHCenter );
  label->setAlignment( AlignVCenter );

  topLayout->addStretch( 10 );
}


#include "optiondialog.moc"
