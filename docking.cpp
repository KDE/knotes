/*
 *              KSCD -- a simpole cd player for the KDE project
 *
 * $Id$
 *
 *              Copyright (C) 1997 Bernd Johannes Wuebben
 *                      wuebben@math.cornell.edu
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "knotes.h"

#include <qtooltip.h>
#include <kwm.h>
#include <kapp.h>

#include "knotes.h"
#include "docking.h"

#define PMERROR(pm) \
  tmp.sprintf(klocale->translate("Could not load %s !"), pm); \
  QMessageBox::warning(this, klocale->translate("Error"), tmp);

//extern QStrList  KPostit::PostitFilesList; // names of all postit files
extern KApplication *mykapp;
//extern QList<KPostit> 	  KPostit::PostitList;    

DockWidget::DockWidget(const char *name): QWidget(0, name, 0) {

  docked = false;

  pos_x = pos_y = 0;

  QString pixdir = mykapp->kde_datadir() + "/knotes/pics/";
  QString tmp;


  if (!picsmall_pixmap.load(pixdir + "knotessmall.xpm")){
    PMERROR("knotessmall.xpm");
  }

  //////////////////////////////////////////////////////////////////
  // popup menu for left mouse button

  popup_m = new QPopupMenu();

  createLeftPopUp();

  connect( popup_m, SIGNAL(activated( int )),SLOT(findKPostit(int)) );
  popup_m->adjustSize();

  ///////////////////////////////////////////////////////////////////
  // right_popup menu for left mouse button

  right_popup_m = new QPopupMenu();
  right_popup_m->insertItem (klocale->translate("Help"),this,SLOT(help()));

  right_popup_m->insertItem(klocale->translate("Change Defaults"),
			    this, SLOT(configureKNotes()));

  right_popup_m->insertSeparator();

  right_popup_m->insertItem (klocale->translate("Exit Knotes"),
			     this, SLOT(exitKNotes()));

  //  connect( right_popup_m, SIGNAL(activated( int )),SLOT(rightAction(int)) );
  right_popup_m->adjustSize();


}

DockWidget::~DockWidget(){


}

void DockWidget::createLeftPopUp(){

  //  printf("In creatLeftPopUp\n");
  popup_m->clear();

  newID = popup_m->insertItem(klocale->translate("New Knote"),
				 this, SLOT(newKNote()));

  popup_m->insertSeparator();

  for ( uint i = 0; i < KPostit::PostitFilesList.count(); i++){
      
    popup_m->insertItem (KPostit::PostitFilesList.at(i));

  }
}

void DockWidget::newKNote(){

  KPostit* postit;

  if(KPostit::PostitFilesList.count() == 0 ){

    KPostit::PostitFilesList.append("knote 1");
    postit = new KPostit(NULL,NULL,0,KPostit::PostitFilesList.last());

    KPostit::PostitList.append(postit); 
    postit->show();
    createLeftPopUp();

  }
  else{

    KPostit::PostitList.first()->newKPostit(); 
  }


}
void DockWidget::exitKNotes(){

  //  printf("in exitKNotes\n");

  if(KPostit::PostitList.count() > 0){

    printf("about to exit via PostitList\n");
    KPostit::PostitList.at(0)->quit();

  }
  else{
      QApplication::exit();
  }

}

void DockWidget::configureKNotes(){


  KPostit* postit;

  if(KPostit::PostitFilesList.count() == 0 ){

    KPostit::PostitFilesList.append("knote 1");
    postit = new KPostit(NULL,NULL,0,KPostit::PostitFilesList.last());

    KPostit::PostitList.append(postit); 


  }

  KPostit::PostitList.at(0)->defaults();
  createLeftPopUp();

}


void DockWidget::help(){

  mykapp->invokeHTMLHelp("","");

}


void DockWidget::dock() {

  if (!docked) {

    // prepare panel to accept this widget
    KWM::setDockWindow (this->winId());

    // that's all the space there is
    this->setFixedSize(24, 24);

    // finally dock the widget
    this->show();
    docked = true;
  }

}

void DockWidget::findKPostit(int i){

  if(KPostit::PostitList.count() > 0 && i > 1){
    KPostit::PostitList.at(0)->findKPostit( i - 2 );
  }

}


void DockWidget::undock() {

  if (docked) {

    // the widget's window has to be destroyed in order 
    // to undock from the panel. Simply using hide() is
    // not enough.
    this->destroy(true, true);

    // recreate window for further dockings
    this->create(0, true, false);

    docked = false;
  }
}

const bool DockWidget::isDocked() {

  return docked;

}

void DockWidget::paintEvent (QPaintEvent *e) {

  (void) e;

  paintIcon();

}

void DockWidget::paintIcon () {

  bitBlt(this, 0, 0, &picsmall_pixmap);


}


void DockWidget::mousePressEvent(QMouseEvent *e) {

  if ( e->button() == (LeftButton || MidButton) ) {

    QPoint point = this->mapToGlobal (QPoint(0,0));

    point  = point - QPoint(30,60);
    popup_m->popup(point);
    popup_m->exec();
    return;
  }

  if ( e->button() == RightButton ) {

    QPoint point = this->mapToGlobal (QPoint(0,0));

    point  = point - QPoint(30,60);
    right_popup_m->popup(point);
    right_popup_m->exec();
  }

}

void DockWidget::toggle_window_state() {

  /*
  // restore/hide connect-window
  if(k != 0L)  {
    if (k->isVisible()){


    QPoint point = k->mapToGlobal (QPoint (0,0));
     pos_x = point.x();
     pos_y = point.y();
     k->hide();
    }
    else {
     k->setGeometry(
		 pos_x, 
		 pos_y,
		 k->width(),
		 k->height());

      k->show();
    }
  }
  */
}





#include "docking.moc"






