
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


#include "mail.h"


Mail::Mail(KPostit *parent, const char *name)
    : QDialog(parent, name,TRUE){


    this->setFocusPolicy(QWidget::StrongFocus);

    frame1 = new QGroupBox(
			   klocale->translate(
			   "Mail Note to:"), this, "frame1");

    recipient = new QLineEdit( this, "recipient");
    recipient->setFocus();

    subject = new QLineEdit( this, "Subject");
    subject->setText(parent->name);

    subjectlabel = new QLabel(this,"subjectlabel");
    subjectlabel->setText(klocale->translate("Subject:"));

    ok = new QPushButton(klocale->translate("Mail"), this, "mail");
    connect(ok, SIGNAL(clicked()), this, SLOT(ok_slot()));

    cancel = new QPushButton(klocale->translate("Cancel"), this, "cancel");
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancel_slot()));

    setFixedSize(330, 160);

}

void Mail::resizeEvent(QResizeEvent *){

    frame1->setGeometry(5, 5, width() - 10, 115);

    cancel->setGeometry(width() - 80, height() - 30, 70, 25);
    ok->setGeometry(width() - 80 - 80 - 10, height() - 30, 70, 25);

    recipient->setGeometry(20, 25, width() - 40, 25);
    subject->setGeometry(20, 80, width() - 40, 25);
    
    subjectlabel->setGeometry(20,55,80,20);
}


void Mail::focusInEvent( QFocusEvent *){

    recipient->setFocus();

}

void Mail::cancel_slot(){
  reject();
}

void Mail::ok_slot(){

  QString str = getRecipient();
  if (str.isEmpty()){
    QMessageBox::warning(
			 this,
			 klocale->translate("Sorry"),
			 klocale->translate("You must specify a Recipient")
			 );
    return;
  }

  accept();
}

#include "mail.moc"
