
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


#ifndef _ALARM_DLG_H_
#define _ALARM_DLG_H_

#include <qdatetime.h> 
#include <kdialogbase.h>

class KPostit;
class BWDateTime;

class AlarmDialog : public KDialogBase
{
  Q_OBJECT

  public:
    AlarmDialog( KPostit *parent, const char *name=0, bool modal=true );
    ~AlarmDialog( void );

    QDateTime getDateTime( void );

  protected slots:
    virtual void slotUser1( void );

  private:
    bool checkDateTime( void );

  private:
    KPostit    *mPostit;
    BWDateTime *mSpins;
    QDateTime  mDateTime;
    bool       mAlarmActive;
};

#endif

