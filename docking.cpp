/*
 *             KNotes
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

#include <kapp.h>
#include <kcmenumngr.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kwm.h>

#include <qtooltip.h>

#include "knotes.h"
#include "docking.h"

//extern QStrList  KPostit::PostitFilesList; // names of all postit files
//extern QList<KPostit> 	  KPostit::PostitList;    

DockWidget::DockWidget(const char *name)
  : QLabel( 0, name, 0 ) 
{
  docked = false;
  pos_x = pos_y = 0;
  left_popup_m = 0;

  picsmall_pixmap = SmallIcon("knotes");
  setPixmap( picsmall_pixmap );

  //////////////////////////////////////////////////////////////////
  // popup menu for left mouse button

  createLeftPopUp();

  ///////////////////////////////////////////////////////////////////
  // right_popup menu for left mouse button

  right_popup_m = new KPopupMenu();
  right_popup_m->insertTitle( picsmall_pixmap, kapp->caption() );
  right_popup_m->insertItem (i18n("Help"),
    this,SLOT(help()));
  right_popup_m->insertItem(i18n("Configuration..."),
    this, SLOT(configureKNotes()));
  right_popup_m->insertSeparator();
  right_popup_m->insertItem (i18n("Quit"),
    this, SLOT(exitKNotes()));
}


DockWidget::~DockWidget()
{
}


void DockWidget::createLeftPopUp( void )
{
  if( left_popup_m == 0 )
  {
    left_popup_m = new KPopupMenu();
    if( left_popup_m == 0 ) { return; }
    connect( left_popup_m, SIGNAL(activated( int )),SLOT(findKPostit(int)) );
  }
  else
  {
    left_popup_m->clear();
  }

  newID = left_popup_m->insertItem(i18n("New Knote"),
    this, SLOT(newKNote()));
  left_popup_m->insertSeparator();
  for( uint i = 0; i < KPostit::PostitFilesList.count(); i++ )
  {      
    left_popup_m->insertItem (KPostit::PostitFilesList[i]);
  }
}


void DockWidget::newKNote( void )
{
  KPostit* postit;
  if(KPostit::PostitFilesList.count() == 0 )
  {
    KPostit::PostitFilesList.append("knote 1");
    postit = new KPostit(NULL,NULL,0,KPostit::PostitFilesList.last());

    KPostit::PostitList.append(postit); 
    postit->show();
    createLeftPopUp();
  }
  else
  {
    KPostit::PostitList.first()->newKPostit(); 
  }
}


void DockWidget::exitKNotes( void )
{
  if(KPostit::PostitList.count() > 0)
  {
    KPostit::PostitList.at(0)->quit();
  }
  else
  {
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


void DockWidget::help( void )
{
  kapp->invokeHTMLHelp("","");
}


void DockWidget::dock( void ) 
{
  if( docked == false ) 
  {
    // prepare panel to accept this widget
    KWM::setDockWindow (this->winId());
    // that's all the space there is
    this->setFixedSize(24, 24);
    // finally dock the widget
    this->show();
    docked = true;
  }
}


void DockWidget::undock( void ) 
{
  if( docked == true ) 
  {
    // new docking method, taken from ksirc (servercontroller.cpp)
    this->hide();
    this->recreate(0x0, 0, QPoint(0,0), FALSE); 
    docked = false;
  }
}


const bool DockWidget::isDocked( void ) 
{
  return docked;
}


void DockWidget::findKPostit(int i)
{
  // convert absolute id to relative index
  i = left_popup_m->indexOf(i);

  if(KPostit::PostitList.count() > 0 && i > 1)
  {
    KPostit::PostitList.at(0)->findKPostit( i - 2 );
  }
}

/*
void DockWidget::paintEvent(QPaintEvent * ) 
{
  paintIcon();
}


void DockWidget::paintIcon () 
{
  bitBlt(this, 0, 0, &picsmall_pixmap);
}
*/

void DockWidget::mousePressEvent(QMouseEvent *e) 
{
  if( KContextMenuManager::showOnButtonPress() == false )
  {
    return;
  }

  if( e->button() == LeftButton ) 
  {
    left_popup_m->exec( mapToGlobal(QPoint(0,0))-QPoint(30,60) );
  }
  else if( e->button() == RightButton ) 
  {
    right_popup_m->exec( mapToGlobal(QPoint(0,0))-QPoint(30,60) );
  }
}


void DockWidget::mouseReleaseEvent(QMouseEvent *e) 
{
  if( KContextMenuManager::showOnButtonPress() == true )
  {
    return;
  }

  if( e->button() == LeftButton ) 
  {
    //left_popup_m->exec( mapToGlobal(QPoint(0,0))-QPoint(30,60) );
    left_popup_m->exec( e->globalPos() );
  }
  else if( e->button() == RightButton ) 
  {
    //right_popup_m->exec( mapToGlobal(QPoint(0,0))-QPoint(30,60) );
    right_popup_m->exec( e->globalPos() );
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
