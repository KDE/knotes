/*

 $Id$

 KNotes - Notes for the KDE Project
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


#include <qbitmap.h>
#include <qtimer.h>
#include <spin.h>

#include "up.xbm"
#include "down.xbm"


QtedSetInt::QtedSetInt(int min, int max, int start_value, int format, 
           QWidget *parent, const char *name) : 
           QWidget(parent, name), _min(min), _max(max),
           _format(format), _up_timer_p(NULL), _dn_timer_p(NULL) {
   if (_max <_min)		// make sure numbers make sense
      _max = _min;
 
   QString str;
   str.sprintf("%d", _min);	// find the max number string length 
   _length = str.length();
   str.sprintf("%d", _max);
   if ((int)str.length() > _length)
      _length = str.length();

   _upButton_p = new QPushButton(this);
   QBitmap up_bm(up_arrow_width, up_arrow_height, up_arrow_bits, TRUE);
   _upButton_p->setPixmap(up_bm);
   _upButton_p->setAutoResize(TRUE);

   _dnButton_p = new QPushButton(this);
   QBitmap dn_bm(dn_arrow_width, dn_arrow_height, dn_arrow_bits, TRUE);
   _dnButton_p->setPixmap(dn_bm);
   _dnButton_p->setAutoResize(TRUE);

   _label_p = new QLabel("Foo", this);
   _label_p->setAlignment(AlignCenter);
   _label_p->setLineWidth(2);
   _label_p->setFrameStyle(QFrame::Panel | QFrame::Sunken);
   _label_p->setBackgroundColor(white);

   value(start_value);
   setSize();
   connect(_upButton_p, SIGNAL(pressed()), this, SLOT(upPressed()));
   connect(_upButton_p, SIGNAL(released()), this, SLOT(upReleased()));
   connect(_dnButton_p, SIGNAL(pressed()), this, SLOT(dnPressed()));
   connect(_dnButton_p, SIGNAL(released()), this, SLOT(dnReleased()));
}

//*****************************************************************************

void QtedSetInt::
setSize(void) {
  int num_wid;// mid_wid;
   int bt_sz = 13;
   int w, mid_w, label_h;
   label_h = fontMetrics().lineSpacing () + 2 * _label_p->frameWidth();

   num_wid = fontMetrics().maxWidth() * _length;
   num_wid += 2 * _label_p->frameWidth(); 

   if (num_wid > bt_sz * 2)
      w = num_wid;
   else 
      w = bt_sz * 2;
   mid_w = w / 2;

   _label_p->setGeometry(0, 0, w, label_h);
   _upButton_p->setGeometry(mid_w - bt_sz, label_h +2, bt_sz, bt_sz);
   _dnButton_p->setGeometry(mid_w, label_h +2, bt_sz, bt_sz);
   setFixedSize(w, label_h + bt_sz +2);
}

//*****************************************************************************

void QtedSetInt::
setText(void) {
   QString str;
   switch (_format) {
      case RightJustified:
         str.sprintf("%*d", _length, _value);
         break;
      case Centered:
         str.sprintf("%d", _value);
         break;
      case ZeroFilled:
         str.sprintf("%0*d", _length, _value);
         break;
   }
   _label_p->setText(str);
}

//*****************************************************************************

void QtedSetInt::
value(int the_value) {
   if (the_value < _min)
      _value = _min;
   else if (the_value > _max)
      _value = _max;
   else
      _value = the_value; 

   setText();
}

//*****************************************************************************

void QtedSetInt::
upPressed() {
   incValue();
   if (_up_timer_p == NULL) {
      _up_timer_p= new QTimer(this);
      connect(_up_timer_p, SIGNAL(timeout()), this, SLOT(upRepeat()));
   }
   if (_value == _max)
      _up_timer_p->stop();
   else
      _up_timer_p->changeInterval((int)(0.25 * 1000));
}

//*****************************************************************************

void QtedSetInt::
upReleased() {
   _up_timer_p->stop();
}

//*****************************************************************************
 
void QtedSetInt::
upRepeat() {
   incValue(); 
   if (_value == _max)
      _up_timer_p->stop();
   else
      _up_timer_p->changeInterval((int)(0.033 * 1000));
}

//*****************************************************************************

void QtedSetInt::
dnPressed() {
   decValue(); 
   if (_dn_timer_p == NULL) {
      _dn_timer_p= new QTimer(this);
      connect(_dn_timer_p, SIGNAL(timeout()), this, SLOT(dnRepeat()));
   }
   if (_value == _min)
      _dn_timer_p->stop();
   else
      _dn_timer_p->changeInterval((int)(0.25 * 1000));
}

//*****************************************************************************

void QtedSetInt::
dnReleased() {
   _dn_timer_p->stop();
}

//*****************************************************************************
 
void QtedSetInt::
dnRepeat() {
   decValue(); 
   if (_value == _min)
      _dn_timer_p->stop(); 
   else
      _dn_timer_p->changeInterval((int)(0.033 * 1000));
}

//*****************************************************************************

void QtedSetInt::
incValue(void) {
   if (_value < _max) {
      _value++;
      setText();
   }
}

//*****************************************************************************

void QtedSetInt::
decValue(void) {
   if (_value > _min) {
      _value--;
      setText();
   }
}

//*****************************************************************************

#include "spin.moc"
