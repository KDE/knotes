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

 (klocale->translate stuff added by didier Belot <dib@avo.fr>)
 
 */

#include "renamedlg.h"
#include <kapp.h>

RenameDlg::RenameDlg( QWidget *parent, const char *name,
		      QString *string,QStrList *list)
  : QDialog( parent, name, TRUE ){

    pstring = string;
    pstrlist = list;
    
    frame = new QGroupBox( klocale->translate("Rename this note to"), this );
    lineNum = new QLineEdit( this );
    this->setFocusPolicy( QWidget::StrongFocus );
    connect(lineNum, SIGNAL(returnPressed()), this, SLOT(selected()));
    
    ok = new QPushButton(klocale->translate("Rename"), this );
    cancel = new QPushButton(klocale->translate("Cancel"), this ); 
    
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ok, SIGNAL(clicked()), this, SLOT(selected()));
    resize(300, 120); 
    
}

void RenameDlg::resizeEvent(QResizeEvent *){

    frame->setGeometry(5, 5, width() - 10, 80);
    cancel->setGeometry(width() - 80, height() - 30, 70, 25);
    ok->setGeometry(width() -80 - 10 - 70, height() - 30, 70, 25);
    lineNum->setGeometry(20, 35, width() - 40, 25);
}

void RenameDlg::focusInEvent( QFocusEvent *){

    lineNum->setFocus();
    lineNum->selectAll();
}

void RenameDlg::selected(){

  *pstring = lineNum->text();
  pstring->stripWhiteSpace();
  if(pstring->isEmpty())
    reject();
  if(*pstring == QString("xyalarms")){

    QMessageBox::warning(
			 this,
			 klocale->translate("Sorry"),
			 klocale->translate(
		     "The name \"xyalarms\" is reserved for internal usage.\n"\
		     "Please choose a different name")
		      );
      return;	

  }
  
  for(pstrlist->first();pstrlist->current();pstrlist->next()){
    if(QString(pstrlist->current()) == *pstring){

      QMessageBox::warning(
		     this,
		     klocale->translate("Sorry"),
		     klocale->translate("A KNotes note with this name already exists\n"\
		     "Please choose a different name")
		     );

      return;	
    }
  }
  
  accept();
}

#include "renamedlg.moc"
