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

 (i18n stuff added by didier Belot <dib@avo.fr>)
 
 */

//
// 1999-12-28 Espen Sand
// Changed to KDialogBase and Qlayouts
//

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qstringlist.h>

#include <klocale.h>
#include <kmessagebox.h>

#include "renamedlg.h"

RenameDialog::RenameDialog( QWidget *parent, const char *name, bool modal,
			    QString *string, QStringList *list )
  : KDialogBase( parent, name, modal, i18n("Rename Note"), Cancel|Ok, Ok )
{
  pstring = string;
  pstrlist = list;
  
  QWidget *page = new QWidget( this ); 
  setMainWidget(page);
  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
  
  QString text = i18n("Rename this note to:");
  QLabel *label = new QLabel( text, page );
  topLayout->addWidget( label );
  
  lineNum = new QLineEdit( page );
  lineNum->setFocus();
  lineNum->setMinimumWidth( fontMetrics().maxWidth()*20 );
  topLayout->addWidget( lineNum );

  topLayout->addSpacing( spacingHint() );
  topLayout->addStretch(10);
}


RenameDialog::~RenameDialog( void )
{
}


void RenameDialog::slotOk( void )
{
  *pstring = lineNum->text();
  pstring->stripWhiteSpace();
  if( pstring->isEmpty() == true )
  {
    reject();
  }

  if( *pstring == "xyalarms" )
  {
    QString msg = i18n(""
      "The name \"xyalarms\" is reserved for internal usage.\n"
      "Please choose a different name");
    KMessageBox::sorry(this, msg );
    return;	
  }

  for(QStringList::Iterator it=pstrlist->begin(); it != pstrlist->end(); it++)
  {
    if( *it == *pstring )
    {
      QString msg = i18n(""
        "A note with this name already exists.\n"
	"Please choose a different name");
      KMessageBox::sorry( this, msg );
      return;	
    }
  }
  
  accept();
}


#include "renamedlg.moc"
