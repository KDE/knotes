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

#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>


#include <klocale.h>
#include <kmessagebox.h>

#include "knotes.h"
#include "mail.h"


Mail::Mail( KPostit *parent, const char *name, bool modal )
  : KDialogBase( parent, name, modal, i18n("Mail Note"), User1|Cancel, 
		 User1, false, i18n("&Mail") ) 
{
  QWidget *page = new QWidget( this ); 
  setMainWidget(page);
  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
  
  QString text = i18n("Mail Note to:");
  QLabel *label = new QLabel( text, page, "mailto" );
  topLayout->addWidget( label );
  
  recipient = new QLineEdit( page, "mailtoedit");
  recipient->setFocus();
  recipient->setMinimumWidth( fontMetrics().maxWidth()*20 );
  topLayout->addWidget( recipient );
  
  text = i18n("Subject:");
  label = new QLabel( text, page, "subject" );
  topLayout->addWidget( label );

  subject = new QLineEdit( page, "subjectedit");
  subject->setMinimumWidth( fontMetrics().maxWidth()*20 );
  subject->setText(parent->name);
  topLayout->addWidget( subject );

  topLayout->addSpacing( spacingHint() );
  topLayout->addStretch(10);
}


Mail::~Mail( void ) 
{
}


QString Mail::getRecipient( void ) 
{
  return recipient->text();
}


QString Mail::getSubject( void ) 
{
  return subject->text();
}


void Mail::slotUser1( void )
{
  QString str = getRecipient();
  if( str.isEmpty() == true )
  {
    KMessageBox::sorry( this, i18n("You must specify a Recipient") );
    return;
  }
  accept();
}


#include "mail.moc"
