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

 (i18n stuff added by didier Belot <dib@avo.fr>)

 */

//
// 1999-12-28 Espen Sand
// Changed to KDialogBase and Qlayouts
//

#include <qgroupbox.h> 
#include <qlayout.h>

#include <klocale.h>
#include <kmessagebox.h>

#include "alarm.h"
#include "bwdatetime.h"
#include "knotes.h"
#include "spin.h"
#include "timer.h"

extern MyTimer* 	mytimer;


AlarmDialog::AlarmDialog( KPostit *parent, const char *name, bool modal )
  : KDialogBase( parent, name, modal, i18n("Alarm"), Cancel|User1, User1 )
{
  mPostit      = parent;
  mAlarmActive = false;
  QDateTime dt = QDateTime::currentDateTime();

  for( mPostit->AlarmList.first(); mPostit->AlarmList.current() != 0;
       mPostit->AlarmList.next() )
  {
    if( mPostit->AlarmList.current()->name == mPostit->name )
    {
      mAlarmActive = true;
      dt = mPostit->AlarmList.current()->dt;
      break;
    }
  }

  QWidget *page = new QWidget( this ); 
  setMainWidget(page);
  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );

  QString text = i18n("Alarm Timer for: %1").arg(mPostit->name);
  QGroupBox *group = new QGroupBox( text, page, "group" );
  topLayout->addWidget( group );

  QVBoxLayout *vlay = new QVBoxLayout( group, spacingHint() );
  vlay->addSpacing( fontMetrics().lineSpacing() );

  mSpins = new BWDateTime( dt, group, "spins" );
  vlay->addWidget( mSpins );

  setButtonText( User1, mAlarmActive == true ? i18n("&Unset") : i18n("&Set") );
}


AlarmDialog::~AlarmDialog( void )
{
}


void AlarmDialog::slotUser1( void )
{
  if( mAlarmActive == true )
  {
    //
    // Remove an alarm from the list and update the note window caption
    //

    mytimer->stop();
    for( mPostit->AlarmList.first(); mPostit->AlarmList.current() != 0;
	 mPostit->AlarmList.next() )
    {
      if( mPostit->AlarmList.current()->name == mPostit->name )
      {
	mPostit->AlarmList.remove(mPostit->AlarmList.current());
	mPostit->setCaption(mPostit->name);
	mPostit->label->setText(mPostit->name.data());
	break;
      }
    }
    mytimer->start();
    reject();
  }
  else if( checkDateTime() == true )
  {
    accept();
  }
}


QDateTime AlarmDialog::getDateTime( void )
{
  return mDateTime;
}


bool AlarmDialog::checkDateTime( void )
{
  if( mSpins->checkDateTime() == false )
  {
    return false;
  }

  QDateTime dt = mSpins->getDateTime();
  if( dt < QDateTime::currentDateTime() )
  {
    QString msg = i18n("I am afraid you already missed your appointment.");
    KMessageBox::sorry( this, msg );
    return false;
  }

  mDateTime = dt;
  return true;
}


#include "alarm.moc"

