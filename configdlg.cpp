
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

 (klocale->translate stuff added by didier Belot <dib@avo.fr>)

 */



#include "configdlg.h"

#include "configdlg.moc"

ConfigDlg::ConfigDlg(QWidget *parent, const char *name,
		     KApplication *mykapp, DefStruct *defstruct)
  : QDialog(parent, name)
{

  mykapp = kapp;
  defst = defstruct;

  box = new QGroupBox(this, "box");
  box->setGeometry(10,10,320,260);
  box->setTitle(i18n("Defaults"));

  label1 = new QLabel(this);
  label1->setGeometry(20,30,135,25);
  label1->setText(klocale->translate("Text Color:"));

  qframe1 = new QFrame(this);
  qframe1->setGeometry(155,30,30,25);	
  qframe1->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  qframe1->setBackgroundColor(defst->forecolor);

  button1 = new QPushButton(this);
  button1->setGeometry(205,30,100,25);
  button1->setText(klocale->translate("Change"));
  connect(button1,SIGNAL(clicked()),this,SLOT(set_fore_color()));

  label2 = new QLabel(this);
  label2->setGeometry(20,65,135,25);
  label2->setText(klocale->translate("Background Color:"));

  qframe2 = new QFrame(this);
  qframe2->setGeometry(155,65,30,25);	
  qframe2->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  qframe2->setBackgroundColor(defst->backcolor);

  button2 = new QPushButton(this);
  button2->setGeometry(205,65,100,25);
  button2->setText(klocale->translate("Change"));
  connect(button2,SIGNAL(clicked()),this,SLOT(set_background_color()));

  /*  button3 = new QPushButton(this);
  button3->setGeometry(255,240,70,25);
  button3->setText(klocale->translate("Help"));
  connect(button3,SIGNAL(clicked()),this,SLOT(help()));
  */
  label5 = new QLabel(this);
  label5->setGeometry(20,105,50,25);
  label5->setText(klocale->translate("Width:"));

  width = new KIntLineEdit(this);
  width->setGeometry(70,105,60,23);
  QString string;
  string.setNum(defst->width);
  width->setText(string);

  label6 = new QLabel(this);
  label6->setGeometry(20,145,50,25);
  label6->setText(klocale->translate("Height:"));

  height = new KIntLineEdit(this);
  height->setGeometry(70,145,60,23);

  QString string2;
  string2.setNum(defst->height);
  height->setText(string2);
  
  label7 = new QLabel(this);
  label7->setGeometry(20,185,100,25);
  label7->setText(klocale->translate("Print Command:"));

  print = new QLineEdit(this);
  print->setGeometry(130,185,180,23);
  print->setText(defst->printcommand.data());

  label8 = new QLabel(this);
  label8->setGeometry(20,225,100,25);
  label8->setText(klocale->translate("Mail Command:"));

  mail = new QLineEdit(this);
  mail->setGeometry(130,225,180,23);
  mail->setText(defst->mailcommand.data());

  QGroupBox *gbox;

  gbox = new QGroupBox(this);
  gbox->setGeometry(150,105,160,65);

  check1 = new QCheckBox( klocale->translate("3d Frame"), gbox );
  check1->setGeometry( 10, 5, 100, 25 );
  check1->setChecked( defst->frame3d );
  connect( check1, SIGNAL( toggled( bool ) ), SLOT( frame3d_slot( bool ) ) );

  check2 = new QCheckBox( klocale->translate("Auto Indent"), gbox );
  check2->setGeometry( 10, 35, 130, 25 );
  check2->setChecked( defst->autoindent );
  connect( check2, SIGNAL( toggled( bool ) ), SLOT( indent_slot( bool ) ) );

  connect(parent,SIGNAL(applyButtonPressed()),SLOT(okButton()));

}

void ConfigDlg::setWidgets(DefStruct *defstruct){

  defst = defstruct;

  qframe1->setBackgroundColor(defst->forecolor);
  qframe2->setBackgroundColor(defst->backcolor);

  QString string;
  string.setNum(defst->width);
  width->setText(string);

  QString string2;
  string2.setNum(defst->height);
  height->setText(string2);

  print->setText(defst->printcommand.data());
  mail->setText(defst->mailcommand.data());

  check1->setChecked( defst->frame3d );
  check2->setChecked( defst->autoindent );

}

void ConfigDlg::help(){

  mykapp->invokeHTMLHelp("","");

}

void ConfigDlg::okButton(){

  printf("entering okButon\n");
    defst->width      = width->getValue();
    defst->height     = height->getValue();

    defst->mailcommand = mail->text();
    defst->printcommand = print->text();
    defst->mailcommand.detach();
    defst->printcommand.detach();
  printf("leaving okButon\n");
  

}
void ConfigDlg::cancelbutton() {
  //  reject();
}

void ConfigDlg::frame3d_slot(bool _par){

  defst->frame3d = _par;

}

void ConfigDlg::indent_slot(bool _par){

  defst->autoindent = _par;

}

void ConfigDlg::set_fore_color(){


  KColorDialog::getColor(defst->forecolor);
  qframe1->setBackgroundColor(defst->forecolor);


}

void ConfigDlg::set_background_color(){


  KColorDialog::getColor(defst->backcolor);
  qframe2->setBackgroundColor(defst->backcolor);


}






