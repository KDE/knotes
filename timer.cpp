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

      for(KPostit::PostitList.first();KPostit::PostitList.current();
	    KPostit::PostitList.next()){
	
	if(
	   KPostit::PostitList.current()->name == entry->name
	   ){
	  
	  exists = TRUE;
	  KPostit::PostitList.current()->setCaption(
		      KPostit::PostitList.current()->name);
	  
	  kwmcom_send_to_kwm( kwm_un_minimize_window_by_id, 
			      KPostit::PostitList.current()->winId(),
			      0L, 0L, 0L, 0L);
	  KPostit::PostitList.current()->raise();
	}

      }

      if(!exists){


	// if this particular kpostit note widget is not alive yet, create it.
	KPostit *t = new KPostit (NULL,NULL,0, 
				  entry->name.copy());
	t->setCaption(entry->name);
	t->show ();
	KPostit::PostitList.append( t );

      }

      QString str;
      str.sprintf("              Alarm for KNote:           \n"\
		  "\n%s",
		  entry->name.data());

      QMessageBox::message("Alarm",str.data(),"OK");


      delete entry;

      KPostit::AlarmList.remove(entry);

    }

    ++it;
  }
  
  this->start();

}

