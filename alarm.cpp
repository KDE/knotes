
/*

 $Id$

 KPostit -- postit Notes for the KDE project

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


#include "alarm.h"
#include "timer.h"
#include "knotes.h"

extern KApplication* 	mykapp;
extern DefStruct 	postitdefaults;
extern MyTimer* 	mytimer;

AlarmDlg::AlarmDlg(KPostit *parent, const char *name)
    : QDialog(parent, name,TRUE){


    postit = parent;
    alarm_is_on = FALSE;

    for(postit->AlarmList.first();postit->AlarmList.current();postit->AlarmList.next()){
      if (postit->AlarmList.current()->name == postit->name){

	alarm_is_on = TRUE;
	qdt = postit->AlarmList.current()->dt;
	break;
      }
    }

    this->setFocusPolicy(QWidget::StrongFocus);

    QString str;
    str.sprintf("Alarm Timer for: %s",postit->name.data());

    frame1 = new QGroupBox(str.data(), this, "frame1");
    
    if(alarm_is_on)
      spins = new  BWDateTime(qdt, this, "spins");
    else
      spins = new  BWDateTime(QDateTime::currentDateTime(), this, "spins");



    spins->move(10,20);
    
    if(alarm_is_on)
      ok = new QPushButton("Unset", this, "mail");
    else
      ok = new QPushButton("Set", this, "mail");

    connect(ok, SIGNAL(clicked()), this, SLOT(ok_slot()));

    cancel = new QPushButton("Cancel", this, "cancel");
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancel_slot()));

    setFixedSize(330, 160);

}

void AlarmDlg::resizeEvent(QResizeEvent *){

    frame1->setGeometry(5, 5, width() - 10, 115);

    cancel->setGeometry(width() - 80, height() - 30, 70, 25);
    ok->setGeometry(width() - 80 - 80 - 10, height() - 30, 70, 25);

    }


void AlarmDlg::focusInEvent( QFocusEvent *){

  //    recipient->setFocus();

}

void AlarmDlg::cancel_slot(){
  reject();
}

void AlarmDlg::ok_slot(){

  if(alarm_is_on){
    alarm_is_on = FALSE;

    mytimer->stop();
    for(postit->AlarmList.first();postit->AlarmList.current();postit->AlarmList.next()){
      if (postit->AlarmList.current()->name == postit->name){

	//	delete postit->AlarmList.current();
	postit->AlarmList.remove(postit->AlarmList.current());
	postit->setCaption(postit->name.data());
	break;
      }
    }
    mytimer->start();
    reject();
    return;
  }


  if(checkDateTime()){
    accept();
  }
  return;
}


QDateTime AlarmDlg::getDateTime(){
  
  return mydatetime;

}

bool AlarmDlg::checkDateTime(){

  QDateTime rdt;

  if(!spins->checkDateTime())
    return FALSE;

  rdt = spins->getDateTime();
  
  if( rdt < QDateTime::currentDateTime()){

    QMessageBox::message("Sorry",
			 "I am afraid you already missed your appointment.","OK");
    return FALSE;
  }

  mydatetime = rdt;
  return TRUE;

}
#include "alarm.moc"

