
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


#ifndef _MAIL_DLG_H_
#define _MAIL_DLG_H_

#include <qgrpbox.h> 
#include <qchkbox.h>
#include <qdialog.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qframe.h>

#include <kcolordlg.h>
#include "knotes.h"

class Mail : public QDialog
{
    Q_OBJECT;

public:

    Mail ( KPostit *parent = 0, const char *name=0);

    QLineEdit 	*recipient;
    QLineEdit 	*subject;
    QLabel 	*subjectlabel;
    QString getRecipient(){return recipient->text();};
    QString getSubject(){return subject->text();};

protected:

    void focusInEvent( QFocusEvent *);
    void resizeEvent ( QResizeEvent *);

private:

    QPushButton *ok, *cancel;
    QGroupBox 	*frame1;



public slots:
     
    void cancel_slot();
    void ok_slot();

};

#endif
