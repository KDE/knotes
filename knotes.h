/*

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

#ifndef __KNOTES__
#define __KNOTES__

#include <qcolor.h>
#include <qdatetime.h>
#include <qdragobject.h>
#include <qfont.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qtextstream.h>

class QLabel;
class QPopupMenu;
class QTimer;
class OptionDialog;


typedef struct _DefStruct{
  QColor forecolor;
  QColor backcolor;
  int 	width;
  int 	height;
  bool 	frame3d;
  bool 	autoindent;
  QFont   font;
  QString mailcommand;
  QString printcommand;
  QString soundcommand;
  bool  playSound;
  bool	onTop;
}DefStruct;

typedef struct _AlarmEntry{
  QString name;
  QDateTime dt;
}AlarmEntry;


class myPushButton: public QPushButton
{
  Q_OBJECT
public:
  myPushButton ( QWidget *parent=0, const char* name=0 );
  ~myPushButton () {}
  bool flat;
  int last_button;
protected:
  void enterEvent( QEvent * );
  void leaveEvent( QEvent * );
  void mousePressEvent( QMouseEvent *e);
  void mouseReleaseEvent( QMouseEvent *e);
  void mouseMoveEvent( QMouseEvent *e);
  void paint( QPainter *_painter );
  void drawButton( QPainter *p ){paint(p);}
  void drawButtonLabel( QPainter *p ){paint(p);}
};


class KPostitMultilineEdit: public QMultiLineEdit{
  Q_OBJECT

public:

  KPostitMultilineEdit(QWidget *parent=0, const char *wname=0);
  bool autoIndentMode;
  int  autoIndentID;

protected:
  void  dragMoveEvent(QDragMoveEvent* event);
  void  dragEnterEvent(QDragEnterEvent* event);
  void  dropEvent(QDropEvent* event);

  void  mouseDoubleClickEvent ( QMouseEvent * e );
  void  keyPressEvent(QKeyEvent *e);
  void  mynewLine();

private:
  QString prefixString(QString string);

signals:
  void gotUrlDrop(const char* url);

};


class KPostit :public QFrame{

  Q_OBJECT

public:

  KPostit(QWidget *parent=0, const char *wname=0,int number=0,
	  QString pname="");
  ~KPostit( void );

  // one instance for all kpostits
  static QList<KPostit>   PostitList;
  static QStringList         PostitFilesList;
  static QList<AlarmEntry> AlarmList;
  static bool dock;

  int number;
  bool hidden;
  QString name;
  QString propertystring;
  QLabel* label;
  myPushButton* mybutton;
  KPostitMultilineEdit* edit;

  QPopupMenu *right_mouse_button;

protected:
  bool  eventFilter( QObject *, QEvent * );
  void resizeEvent( QResizeEvent * );
  void  closeEvent( QCloseEvent * );


public slots:

  void  set3DFrame(bool enable);
  void  toggleFrame();
  void  toggleDock();
  void  set_colors();
  void  set_background_color();
  void  set_foreground_color();
  void  clear_text();
  bool  loadnotes();
  bool  savenotes();
  void  quit();
  void  undo();
  void  redo();
  void  insertDate();
  void  insertNetFile( const char *_url);
  bool  insertFile(const QString &filename);
  void  RMBActivated(int);
  void  close();
  void  selectFont();
  void  findKPostit(int );
  void  newKPostit();
  void  renameKPostit();
  void  deleteKPostit();
  void  hideKPostit();
  void  insertCalendar();
  void  mail();
  void  print();
  void  save_all();
  void  defaults();
  void  setAlarm();
  void  help();
  void  toggleIndentMode();
  void  toggleOnTopMode();
  void  toDesktop(int);
  void  toggleSticky();

  private slots:
    void configurationChanged( const DefStruct &state );


  private:
    void  setAutoIndent();
    void  setNoAutoIndent();
    void  setOnTop(bool enable);

  private:  
    QFont font;
    QPopupMenu *colors;
    QPopupMenu *options;
    QPopupMenu *operations;
    QPopupMenu *desktops;
    int  frame3dID;
    int onTopID;
    int  dockID;
    QColor forecolor;
    QColor backcolor;

    QTimer *timer;
    bool frame3d;

    QPoint pointerOffset;
    bool dragging;
    int sticky_id;

    OptionDialog *mOptionDialog;

};

class sessionWidget : public QWidget {
  Q_OBJECT
public:
  sessionWidget();
  ~sessionWidget() {};
public slots:
  void wm_saveyourself();
};

#endif


