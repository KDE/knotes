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



#include "timer.h"
#include "knotes.h"
#include <kwm.h>


extern KApplication* 	mykapp;
extern DefStruct 	postitdefaults;

void MyTimer::start(){
  
  startTimer(1000);

}

void MyTimer::stop(){
  
  killTimers();

}
void MyTimer::timerEvent( QTimerEvent * ){

  this->stop();

  QDateTime qdt = QDateTime::currentDateTime();

  //  printf("In Timer counter:%d\n",KPostit::AlarmList.count());
  
  QListIterator<AlarmEntry> it(KPostit::AlarmList);
  AlarmEntry* entry;

  while ( (entry=it.current()) ) {         

    if(entry->dt < qdt){

      QApplication::beep();
      mykapp->processEvents();
      sleep(1);
      QApplication::beep();
      mykapp->processEvents();
      sleep(1);
      QApplication::beep();


      // now we need to reset the Caption of the PostitWindow whose
      // alarm expired

      bool exists = FALSE;

      KPostit *t = 0;

      for(KPostit::PostitList.first();KPostit::PostitList.current();
	    KPostit::PostitList.next()){
	
	if(
	   KPostit::PostitList.current()->name == entry->name
	   ){
	  
	  exists = TRUE;
	  t = KPostit::PostitList.current();
	  t->setCaption(QString(klocale->translate("Note: ")) +
			KPostit::PostitList.current()->name);
	  t->label->setText(KPostit::PostitList.current()->name);

	  if( KPostit::PostitList.current()->hidden == true){
	    KPostit::PostitList.current()->hidden = false;
	    if(KPostit::PostitList.current()->propertystring != (QString) "")
	      KWM::setProperties(KPostit::PostitList.current()->winId(),
				 KPostit::PostitList.current()->propertystring);
	    
	  }
	  KPostit::PostitList.current()->show();
	  KWM::activate(KPostit::PostitList.current()->winId());
	}

      }

      if(!exists){


	// if this particular kpostit note widget is not alive yet, create it.
	t = new KPostit (NULL,NULL,0, 
			 entry->name.copy());
	t->setCaption(QString(klocale->translate("Note: ")) + entry->name);
	t->label->setText(entry->name);
	t->show ();
	KPostit::PostitList.append( t );

      }

      QString str;
      str.sprintf("Alarm for KNote:\n"\
		  "\n%s",
		  entry->name.data());

      QMessageBox::information(
			       t, 
			       "Alarm",
			       str.data()
			       );


      delete entry;

      KPostit::AlarmList.remove(entry);

    }

    ++it;
  }
  
  this->start();

}

