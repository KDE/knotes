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


#ifndef _BW_SPIN_H_
#define _BW_SPIN_H_


#include <qlabel.h>
#include <qpushbt.h>

class QtedSetInt : public QWidget {
   Q_OBJECT
   private:
      int _value;
      int _min, _max;
      int _format, _length;
      QTimer *_up_timer_p, *_dn_timer_p;
      QLabel *_label_p;
      QPushButton *_upButton_p, *_dnButton_p;

      void setText(void);
      void incValue(void);
      void decValue(void);
      void setSize(void);
   public:
      enum { ZeroFilled, Centered, RightJustified };
      QtedSetInt(int min, int max, int value, int format = Centered, 
                 QWidget *parent=0, const char *name=0);
      void value(int value);
      int value(void) { return (_value); }
   private slots:
      void upPressed();
      void upReleased();
      void dnPressed();
      void dnReleased();

      void upRepeat();
      void dnRepeat();
};

#endif

