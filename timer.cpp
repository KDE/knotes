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
#include <klocale.h>
#include <kprocess.h>
#include <qlabel.h>

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

      if (postitdefaults.playSound && !postitdefaults.soundcommand.isEmpty()) {
	playSound(postitdefaults.soundcommand);
      }
      else {
	QApplication::beep();
	kapp->processEvents();
	sleep(1);
	QApplication::beep();
	kapp->processEvents();
	sleep(1);
	QApplication::beep();
      }

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
	  t->setCaption(i18n("Note: ") +
			KPostit::PostitList.current()->name);
	  t->label->setText(KPostit::PostitList.current()->name);

	  if( KPostit::PostitList.current()->hidden == true){
	    KPostit::PostitList.current()->hidden = false;
	    if(KPostit::PostitList.current()->propertystring != (QString) "")
		KPostit::PostitList.current()->setGeometry(KWM::setProperties(KPostit::PostitList.current()->winId(),
				 KPostit::PostitList.current()->propertystring));
	
	  }
	  KPostit::PostitList.current()->show();
	  KWM::activate(KPostit::PostitList.current()->winId());
	}

      }

      if(!exists){


	// if this particular kpostit note widget is not alive yet, create it.
	t = new KPostit (NULL,NULL,0,
			 entry->name.copy());
	t->setCaption(i18n("Note: ") + entry->name);
	t->label->setText(entry->name);
	t->show ();
	KPostit::PostitList.append( t );

      }

      QString str;
      str = i18n("Alarm for KNote:\n\n%1")
		  .arg(entry->name);

      QMessageBox::information(
			       t,
			       i18n("Alarm"),
			       str,
			       i18n("OK"));


      delete entry;

      KPostit::AlarmList.remove(entry);

    }

    ++it;
  }

  this->start();

}

// this belongs somewhere in KApplication if you ask me
void MyTimer::playSound(  QString cmd )
{
  QString param;
  KProcess p;
  int argIdx;
  
  cmd.simplifyWhiteSpace();

  while( !cmd.isEmpty() )
  {
    argIdx = cmd.find(" ");
    param = cmd.left( argIdx );
    
    p << param;

    if (argIdx == -1)
      argIdx = cmd.length();
    
    cmd = cmd.remove( 0, argIdx + 1 );
  }
  
  if (!p.start(KProcess::DontCare, KProcess::NoCommunication))
    QApplication::beep();
  
}


void SaveTimer::start(){

  startTimer(15*60*1000);

}

void SaveTimer::stop(){

  killTimers();

}
void SaveTimer::timerEvent( QTimerEvent * ){

    for( KPostit::PostitList.first();
	 KPostit::PostitList.current();
	 KPostit::PostitList.next()
	 ){

      KPostit::PostitList.current()->savenotes();

    }

}
