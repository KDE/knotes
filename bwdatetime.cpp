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

 This class is based on a class taken from
 qtremind - an X windows appoint reminder program.
 Copyright (C) 1997  Tom Daley

 (klocale->translate stuff added by didier Belot <dib@avo.fr>)

*/


#include <bwdatetime.h>
#include <kapp.h>


BWDateTime:: BWDateTime(QDateTime qdt, QWidget *parent, const char *name) 
  : QWidget(parent, name){
   
   date_notvalid = FALSE;
   time_notvalid = FALSE;

   daylabel = new QLabel(klocale->translate("Day:"),this);
   daylabel->setGeometry(10,10,40,25);

   day= new QtedSetInt(1, 31,  qdt.date().day(),QtedSetInt::RightJustified, this);
   day->setGeometry(50, 15, day->width(), day->height());

   monthlabel = new QLabel(klocale->translate("Month:"),this);
   monthlabel->setGeometry(90,10,60,25);

   month = new QtedSetInt(1, 12,  qdt.date().month(),
			  QtedSetInt::RightJustified, this);
   month->setGeometry(140, 15, month->width(), month->height());

   yearlabel = new QLabel(klocale->translate("Year:"),this);
   yearlabel->setGeometry(180,10,60,25);

   year  = new QtedSetInt(1, 3000,qdt.date().year(), 	
			  QtedSetInt::RightJustified, this);
   year->setGeometry(220, 15, year->width(), year->height());


   int myhour = qdt.time().hour();

   if (myhour > 12)
     myhour -= 12;
   if (myhour == 0)
     myhour = 12;

   timelabel = new QLabel(klocale->translate("Time:"),this);
   timelabel->setGeometry(10,50,40,25);
   hour  = new QtedSetInt(1, 12  ,myhour, 	
			  QtedSetInt::RightJustified, this);
   hour->setGeometry(50, 55, hour->width(), hour->height());

   minute= new QtedSetInt(0, 59,  qdt.time().minute() , 
			  QtedSetInt::ZeroFilled, this);
   minute->setGeometry(90, 55, minute->width(), minute->height());


   ampm = new QButtonGroup(this);
   
   am = new QRadioButton("AM", ampm);   
   pm = new QRadioButton("PM", ampm);   

   if(qdt.time().hour() < 12){

     pm->setChecked(FALSE);
     am->setChecked(TRUE); 

   }
   else{

     pm->setChecked(TRUE);
     am->setChecked(FALSE);

   }

   QSize size = am->sizeHint();
   
   ampm->setGeometry(140, 55, size.width(), minute->height());

   int half = minute->height() / 2;
   am->setGeometry(0, 0, size.width(), half);
   pm->setGeometry(0, half, size.width(), half);

   resize(300,95);

}



void BWDateTime::setTime(QDateTime dt) {

   day	->value(dt.date().day());
   month->value(dt.date().month());
   year	->value(dt.date().year());

   int myhour = dt.time().hour();

   if (myhour > 12)
     myhour -= 12;

   hour	->value(myhour);
   minute->value(dt.time().minute());
   
   if(dt.time().hour() < 12)
     am->setChecked(TRUE);
   else
     pm->setChecked(FALSE);

}



bool BWDateTime::checkDateTime(){

 QDate rdate;
 QTime rtime;

 time_notvalid = FALSE;
 date_notvalid = FALSE;

 int myhour = hour->value();

 if(pm->isChecked() && (myhour != 12)) // 12 pm is 12 hours
   myhour += 12;

 if(!pm->isChecked() && (myhour ==12)) //12 am is 0 hours
   myhour = 0; 

 if(QTime::isValid(myhour,minute->value(),0)){
    rtime.setHMS(myhour,minute->value(),0);
  }
  else{
    QMessageBox::warning(
			 this,
			 klocale->translate("Sorry"),
			 klocale->translate("The Time you selected is invalid")
			 );

    time_notvalid = TRUE;
    return FALSE;

  }

  if(QDate::isValid(year->value(),month->value(),day->value())){
    rdate.setYMD(year->value(),month->value(),day->value());
  }
  else{
    QMessageBox::warning(
			 this,
			 klocale->translate("Sorry"),
			 klocale->translate("The Date you selected is invalid")
			 );

    date_notvalid = TRUE;
    return FALSE;
  }


  QDateTime rdt(rdate,rtime);
  mydatetime = rdt;

  return TRUE;

}

QDateTime BWDateTime::getDateTime(void) { 

  checkDateTime();

  return mydatetime; 
}

