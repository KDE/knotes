/*

 $ Id: $  

 KPostit - postit notes for the KDE Project
 Copyright (C) 1997 Bernd Johannes Wuebben
 
 wuebben@math.cornell.edu
 wuebben@kde.org

 This class is a modified version of a class found in:

 qtremind - an X windows appoint reminder program.
 Copyright (C) 1997  Tom Daley

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


#ifndef BW_DATE_TIME_H_
#define BW_DATE_TIME_H_

#include <qlabel.h>
#include <qbttngrp.h>
#include <qradiobt.h> 
#include <qdatetm.h>
#include <qmsgbox.h>

#include "spin.h"

class BWDateTime : public QWidget {

public:

  BWDateTime(QDateTime qdt, QWidget *parent=0, const char *name=0);
  
  void setTime(QDateTime qdtime);
  QDateTime getDateTime();
  bool checkDateTime();

  bool time_notvalid;
  bool date_notvalid;

private:
  
  QtedSetInt* hour;
  QtedSetInt* minute;
  QtedSetInt* month;
  QtedSetInt* year;
  QtedSetInt* day;
  
  QRadioButton * am, *pm;
  QButtonGroup * ampm;
  QLabel* daylabel;
  QLabel* monthlabel;
  QLabel* yearlabel;
  QLabel* timelabel;
  QDateTime mydatetime;

};

#endif

