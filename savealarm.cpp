
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


#include <qfile.h>

#include <kstddirs.h>
#include <kdebug.h>

#include "knotes.h"

bool savealarms()
{
  QString alarmfile( locateLocal( "appdata", "xyalarms/knotesalarms") );

  QFile file(alarmfile);

  kbDebug() << "save: " << alarmfile;

  if( !file.open( IO_WriteOnly | IO_Truncate )) {
    return FALSE;
  }

  QDataStream d(&file);
  
  d << (short)KPostit::AlarmList.count();

  for( KPostit::AlarmList.first();  KPostit::AlarmList.current(); 
       KPostit::AlarmList.next()){

    d << KPostit::AlarmList.current()->name;
    d << KPostit::AlarmList.current()->dt;

  }

  
  file.close();

  return TRUE;
}

bool readalarms(){

  QString alarmfile( locateLocal( "appdata", "xyalarms/knotesalarms") );

  QFile file(alarmfile);

  kbDebug() << "save: " << alarmfile;

  if( !file.open( IO_ReadOnly )) {
    return FALSE;
  }

  QDataStream d(&file);
  
  short int count;
  AlarmEntry *entry;

  d >>  count ;

  for( int i = 0 ; i < count ; i++)
  {
    QString sname;
    QDateTime sdt;

    d >> sname;
    d >> sdt;

    //    kbDebug() << sname;
    
    entry 	= new AlarmEntry;
    entry->name = sname;
    entry->dt 	= sdt;

    KPostit::AlarmList.append(entry);
  }
  
  file.close();

  return TRUE;
}
