
/*

 $Id$

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

 */


#include "fontdlg.h"

#include "fontdlg.moc"

FontDlg::FontDlg(QWidget *parent=0, const char *name=0,
		     KApplication *mykapp=NULL,DefStruct *defstruct=NULL)
  : QDialog(parent, name)
{

  mykapp = kapp;
  defst = defstruct;

  box = new QGroupBox(this, "box");
  box->setGeometry(10,10,320,260);
  box->setTitle("Set Default Font");

  button = new QPushButton(this);
  button->setGeometry(235,225,80,25);
  button->setText("Change");
  connect(button,SIGNAL(clicked()),this,SLOT(setFont()));

  familylabel = new QLabel(this);
  familylabel->setGeometry(20,65,135,25);
  familylabel->setText("Family:");

  familylabeldisp = new QLabel(this);
  familylabeldisp->setGeometry(130,65,150,25);	
  familylabeldisp->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  familylabeldisp->setText(defst->font.family());


  sizelabel = new QLabel(this);
  sizelabel->setGeometry(20,105,100,25);
  sizelabel->setText("Point Size:");

  sizelabeldisp = new QLabel(this);
  sizelabeldisp->setGeometry(130,105,60,25);	
  sizelabeldisp->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QString size;
  size.setNum(defst->font.pointSize());
  sizelabeldisp->setText(size);

  stylelabel = new QLabel(this);
  stylelabel->setGeometry(20,140,80,25);
  stylelabel->setText("Style:");

  stylelabeldisp = new QLabel(this);
  stylelabeldisp->setGeometry(130,145,80,23);
  stylelabeldisp->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if(defst->font.italic())
    stylelabeldisp->setText("Italic");
  else
    stylelabeldisp->setText("Roman");

  weightlabel = new QLabel(this);
  weightlabel->setGeometry(20,180,80,25);
  weightlabel->setText("Weight:");

  weightlabeldisp = new QLabel(this);
  weightlabeldisp->setGeometry(130,185,80,23);
  weightlabeldisp->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if(defst->font.bold())
    weightlabeldisp->setText("Bold");
  else
    weightlabeldisp->setText("Normal");

  connect(parent,SIGNAL(applyButtonPressed()),SLOT(okButton()));

}


void FontDlg::help(){

  mykapp->invokeHTMLHelp("","");

}

void FontDlg::okButton(){



}
void FontDlg::cancelbutton() {
  reject();
}


void FontDlg::setFont(){


  KFontDialog::getFont(defst->font);

  familylabeldisp->setText(defst->font.family());

  if(defst->font.bold())
    weightlabeldisp->setText("Bold");
  else
    weightlabeldisp->setText("Normal");

  if(defst->font.italic())
    stylelabeldisp->setText("Italic");
  else
    stylelabeldisp->setText("Roman");

  QString size;
  size.setNum(defst->font.pointSize());
  sizelabeldisp->setText(size);
}







