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

 (i18n stuff added by didier Belot <dib@avo.fr>)

*/

#include <qlayout.h>
#include <qspinbox.h>

#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "bwdatetime.h"


BWDateTime:: BWDateTime(QDateTime qdt, QWidget *parent, const char *name)
  : QWidget(parent, name)
{
   date_notvalid = FALSE;
   time_notvalid = FALSE;

   daylabel = new QLabel(i18n("Day:"),this);
   day = new QSpinBox(1, 31, 1, this);
   day->setValue(qdt.date().day());

   monthlabel = new QLabel(i18n("Month:"), this);
   month = new QSpinBox(1, 12, 1, this);
   month->setValue(qdt.date().month());

   yearlabel = new QLabel(i18n("Year:"),this);
   year = new QSpinBox(1, 3000, 1, this);
   year->setValue(qdt.date().year());

   int myhour = qdt.time().hour();
   if (myhour > 12)
     myhour -= 12;
   if (myhour == 0)
     myhour = 12;

   timelabel = new QLabel(i18n("Time:"),this);
   // this stuff should be replaced with a KTimeSpinBox
   hour = new QSpinBox(1, 12, 1, this);
   hour->setValue(myhour);
   minute = new QSpinBox(0, 59, 1, this);
   minute->setValue(qdt.time().minute());
 
   ampm = new QButtonGroup(this);
   ampm->setFrameStyle( QFrame::NoFrame );

   QVBoxLayout *hlay = new QVBoxLayout( ampm, 0, KDialog::spacingHint() );
   
   am = new QRadioButton(i18n("AM"), ampm);
   pm = new QRadioButton(i18n("PM"), ampm);

   if(qdt.time().hour() < 12)
   {
     pm->setChecked(FALSE);
     am->setChecked(TRUE);
   }
   else
   {
     pm->setChecked(TRUE);
     am->setChecked(FALSE);
   }
   hlay->addWidget(am);
   hlay->addWidget(pm);

   // layout management (pfeiffer)
   QGridLayout *glay = new QGridLayout( this, 6, 3, KDialog::spacingHint() );
   glay->addWidget(daylabel,   0, 0, AlignRight);
   glay->addWidget(day,        0, 1);
   glay->addWidget(monthlabel, 0, 2, AlignRight);
   glay->addWidget(month,      0, 3);
   glay->addWidget(yearlabel,  0, 4, AlignRight);
   glay->addWidget(year,       0, 5);

   glay->addRowSpacing( 1, KDialog::spacingHint() );

   glay->addWidget(timelabel, 2, 0, AlignRight);
   glay->addWidget(hour,      2, 1);
   glay->addWidget(minute,    2, 2);
   glay->addMultiCellWidget(ampm, 2, 2, 3, 5 );
}



void BWDateTime::setTime(QDateTime dt) {

   day->setValue(dt.date().day());
   month->setValue(dt.date().month());
   year->setValue(dt.date().year());

   int myhour = dt.time().hour();

   if (myhour > 12)
     myhour -= 12;

   hour->setValue(myhour);
   minute->setValue(dt.time().minute());

   if(dt.time().hour() < 12)
     am->setChecked(TRUE);
   else
     pm->setChecked(FALSE);

}



bool BWDateTime::checkDateTime( void )
{
  QDate rdate;
  QTime rtime;

  time_notvalid = FALSE;
  date_notvalid = FALSE;

  int myhour = hour->value();

  if(pm->isChecked() && (myhour != 12)) // 12 pm is 12 hours
    myhour += 12;

  if(!pm->isChecked() && (myhour ==12)) //12 am is 0 hours
    myhour = 0;

  if(QTime::isValid(myhour,minute->value(),0))
  {
    rtime.setHMS(myhour,minute->value(),0);
  }
  else
  {
    KMessageBox::sorry( this, i18n("The Time you selected is invalid") );
    time_notvalid = TRUE;
    return FALSE;
  }

  if(QDate::isValid(year->value(),month->value(),day->value()))
  {
    rdate.setYMD(year->value(),month->value(),day->value());
  }
  else
  {
    KMessageBox::sorry( this, i18n("The Date you selected is invalid") );
    date_notvalid = TRUE;
    return FALSE;
  }

  QDateTime rdt(rdate,rtime);
  mydatetime = rdt;
  return TRUE;
}

QDateTime BWDateTime::getDateTime( void ) 
{
  checkDateTime();
  return mydatetime;
}
