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

//
// 1999-12-28 Espen Sand
// Changed to KDialogBase and Qlayouts
//


#ifndef __KPOSTIT_RENAME__
#define __KPOSTIT_RENAME__

#include <kdialogbase.h>
class QStringList;


class RenameDialog : public KDialogBase
{
  Q_OBJECT

  public:
  
    RenameDialog( QWidget *parent=0, const char *name=0, bool modal=true,
	          QString *string=0, QStringList *list=0 );
    ~RenameDialog( void );
   
  protected slots:
    virtual void slotOk( void );

  private:
    QLineEdit   *lineNum;
    QString     *pstring;
    QStringList *pstrlist;
};

#endif

