
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


#include <dirent.h>

#include "knotes.h"
#include "configdlg.h"
#include "fontdlg.h"
#include "mail.h"
#include "alarm.h"
#include "timer.h"
#include "renamedlg.h"
#include "version.h"

#include <X11/Xlib.h>

#include "knotes.moc"

void findPostitFiles();

QList<KPostit> 	  KPostit::PostitList;      // pointers to all postit objects
QStrList 	  KPostit::PostitFilesList; // names of all postit files
QList<AlarmEntry> KPostit::AlarmList;

KApplication* 	mykapp;
DefStruct 	postitdefaults;

static void 	cleanup(int);
void 		catchSignals();
void 	        readSettings();
void 	        writeSettings();

QString 	pidFile;
long 		window_id;
QString 	tmpFile;

MyTimer* 	mytimer;

extern bool     savealarms();
extern bool 	readalarms();

KPostit::KPostit(QWidget *parent, const char *myname,int  _number, QString pname)
  : QMultiLineEdit(parent, myname){


    number = _number; 	// index in popup. Not used anymore, but I'll leave it in
                        // the structure for now.
    name = pname;
    name.detach(); 	// name of postit and name on the popup

    kfm = 0L;

    //set the defaults
    forecolor 		= postitdefaults.forecolor;
    backcolor 		= postitdefaults.backcolor;
    autoIndentMode 	= postitdefaults.autoindent;
    font 		= postitdefaults.font;
    frame3d 		= postitdefaults.frame3d;

    resize(postitdefaults.width,postitdefaults.height);

    loadnotes();

    colors =  	new QPopupMenu ();

    colors->insertItem("Text Color",this, SLOT(set_foreground_color()));
    colors->insertItem("Background Color",this, SLOT(set_background_color()));

    operations =  	new QPopupMenu ();
    operations->insertItem("Clear",this, SLOT(clear_text()));

    operations->insertSeparator();
    operations->insertItem ("New Note", this, 	
				    SLOT(newKPostit()));
    operations->insertItem ("Delete Note", this, 	
				    SLOT(deleteKPostit()));
    operations->insertItem ("Rename Note",this,SLOT(renameKPostit()));
    operations->insertSeparator();
    operations->insertItem ("Alarm ...", this, 	
				    SLOT(setAlarm()));
    operations->insertSeparator();
    operations->insertItem ("Calendar", this, 	
				    SLOT(insertCalendar()));
    operations->insertItem ("Mail Note ...", this, 	
				    SLOT(mail()));
    operations->insertItem ("Print Note", this, 	
				    SLOT(print()));

    options = new QPopupMenu();
    options->setCheckable(TRUE);
    options->setFont(QFont("Helvetica",12));
    frame3dID = options->insertItem("3D Frame",this, SLOT(toggleFrame()));
    autoIndentID = options->insertItem("Auto Indent",this, 
				       SLOT(toggleIndentMode()));
    options->insertItem("Font",this, SLOT(selectFont()));
    options->insertItem("Colors",colors);
    options->insertSeparator();
    options->insertItem("Change Defaults ...",this, SLOT(defaults()));

    operations->insertSeparator();
    operations->insertItem ("Help",this,SLOT(help()));

    operations->insertSeparator();
    operations->insertItem ("Options",options);

    right_mouse_button = new QPopupMenu;

    for ( uint i = 0; i < PostitFilesList.count(); i++){
      
      int k = right_mouse_button->insertItem (PostitFilesList.at(i));
      k = k;
    }

    connect( right_mouse_button, SIGNAL(activated( int )), 
	     SLOT(findKPostit(int)) );


    right_mouse_button->insertSeparator();
    right_mouse_button->insertItem ("Insert Date", this, 	
				    SLOT(insertDate()));
    right_mouse_button->insertSeparator();
    right_mouse_button->insertItem ("Operations",operations);

    right_mouse_button->insertSeparator();
    right_mouse_button->insertItem ("Quit", this,
				    SLOT(quit()));

    installEventFilter( this );     

    kwmcom_init(qt_xdisplay(), winId());
    window_id = winId();

    KDNDDropZone * dropZone = new KDNDDropZone( this , DndURL);
    connect( dropZone, SIGNAL( dropAction( KDNDDropZone *) ), 
	   this, SLOT( slotDropEvent( KDNDDropZone *) ) );


    if(!frame3d){
      setNoFrame();
      options->changeItem("3D Frame",frame3dID);
    }
    else{
      set3DFrame();
      options->changeItem("No Frame",frame3dID);
    }

    if(!autoIndentMode){
      setNoAutoIndent();
      options->setItemChecked(autoIndentID,FALSE);
    }
    else{
      setAutoIndent();
      options->setItemChecked(autoIndentID,TRUE);
    }

    set_colors();
    setFont(font);

    bool have_alarm = FALSE;

    QListIterator<AlarmEntry> it( AlarmList );
    AlarmEntry *entry;                      

    while ( (entry=it.current()) ) {         
      if(entry->name == name){
	have_alarm = TRUE;
	break;
      }
      ++it;
    }

    if (have_alarm)
      setCaption(name + " (A)");
    else
      setCaption(name);


}

void KPostit::toggleshow(){
  
}


void KPostit::clear_text(){

  this->clear();	
}

void KPostit::selectFont(){
 
  QFont myfont = QWidget::font();
  KFontDialog::getFont(myfont);
  this->setFont(myfont);
  font = myfont;
  
}

void KPostit::quit(){

  for(PostitList.first();PostitList.current();PostitList.next()){

    if (!PostitList.current()->savenotes()){
      int result = QMessageBox::query ("Sorry", 
				       "Could not save a KNote.\n"\
				       "Quit anyways?");

      if (!result){
	return;         
      }
    }
  }	

  remove( pidFile.data() );
  writeSettings();
  if(!savealarms()){
    QString str;
    str.sprintf("Could not save KNote Alarms\n");
    QMessageBox::message("Sorry",str.data(),"OK");
  }
  QApplication::exit();

}


void KPostit::help(){

  mykapp->invokeHTMLHelp("","");

}
void KPostit::keyPressEvent(QKeyEvent *e){

  if (e->key() == Key_Tab)
    insertChar('\t');

  if(e->key() == Key_Return || e->key() == Key_Enter){
    
    mynewLine();
    return;
  }

  QMultiLineEdit::keyPressEvent(e);

}

void KPostit::mynewLine(){

  if (isReadOnly())
    return;

  if(!autoIndentMode){ 
    newLine();
    return;
  }

  int line,col;
  bool found_one = false;

  getCursorPosition(&line,&col);
  
  QString string, string2;

  while(line >= 0){

    string  = textLine(line);
    string2 = string.stripWhiteSpace();

    if(!string2.isEmpty()){
      string = prefixString(string);
      found_one = TRUE;
      break;
    }

    line --;
  }
      
  // string will now contain those whitespace characters that I need to insert
  // on the next line. 

  if(found_one){

    // don't ask my why I programmed it this way. I am quite sick of the Qt 1.2
    // MultiLineWidget -- It is anoyingly buggy. 
    // I have to put in obscure workarounds all over the place. 

    focusOutEvent(&QFocusEvent(Event_FocusOut));
    newLine();
    
    for(uint i = 0; i < string.length();i++){
      insertChar(string.data()[i]);
    }

    // this doesn't work. in Qt 1.2:
    // insertAt(string.data(),line + 1,0);

    focusInEvent(&QFocusEvent(Event_FocusIn));

  }
  else{
    newLine();
  }
}

QString KPostit::prefixString(QString string){
  
  // This routine return the whitespace before the first non white space
  // character in string. This is  used in mynewLine() for indent mode.
  // It is assumed that string contains at least one non whitespace character
  // ie \n \r \t \v \f and space
  
  int size = string.size();
  char* buffer = (char*) malloc(size + 1);
  strncpy (buffer, string.data(),size - 1);
  buffer[size] = '\0';

  int i;
  for (i = 0 ; i < size; i++){
    if(!isspace(buffer[i]))
      break;
  }

  buffer[i] = '\0';

  QString returnstring = buffer;
  
  free(buffer);
  return returnstring;

}

void KPostit::insertDate(){
  
  int line, column;

  QString string;
  QDateTime dt = QDateTime::currentDateTime();
  string = dt.toString();

  this->getCursorPosition(&line,&column);
  this->insertAt(string,line,column);
 
}

void KPostit::mail(){

  Mail* maildlg ;
  
  maildlg = new Mail(this,"maildialog");

  if(!maildlg->exec()){
    return;
  }

  mykapp->processEvents();
  mykapp->flushX();

  FILE* mailpipe;
  
  QString cmd;
  cmd = postitdefaults.mailcommand.copy();
  cmd = cmd.sprintf(postitdefaults.mailcommand.data(),
		    maildlg->getSubject().data(),maildlg->getRecipient().data());

  /*  printf("%s\n",cmd.data());*/

  delete maildlg;

  mailpipe = popen(cmd.data(),"w");
  //  printf("popen %d\n",(int) mailpipe);

  if(mailpipe == NULL){
    QString str;
    str.sprintf("Could not pipe the contents of this KNote into:\n %s",cmd.data());
    QMessageBox::message("Sorry",str.data(),"OK");
    return;
  }

  QTextStream t(mailpipe,IO_WriteOnly );

  int line_count = numLines();

  for(int i = 0 ; i < line_count ; i++){
    t << textLine(i) << '\n';
  }
  pclose(mailpipe);
  
  //int exitstatus = pclose(mailpipe);  
  //  printf("pclose %d\n",exitstatus);



}

void KPostit::insertCalendar(){

  FILE* calfile;
  calfile = popen("cal","r");

  QTextStream t(calfile,IO_ReadOnly );

  while ( !t.eof() ) {
    QString s = t.readLine();
    if(!t.eof())
      this->insertLine( s );
  }  
  pclose(calfile);
  repaint();
}

void KPostit::setAlarm(){

  AlarmDlg* alarm;

  alarm = new AlarmDlg(this,"alarmdlg");
  
  if(alarm->exec() == QDialog::Rejected)
    return;

  QDateTime qdt = alarm->getDateTime();

  AlarmEntry* entry;
  entry = new AlarmEntry;
  entry->name = this->name.copy();
  entry->dt = qdt;

  AlarmList.append(entry);
  QString str;
  str.sprintf("%s (A)",name.data());
  setCaption(str.data());

  //  QDate date = qdt.date();
  //  QTime time = qdt.time();
  //  printf("%d %d %d %d %d %d\n", date.day(),date.month(),date.year(),
  //	 time.hour(),time.minute(),time.second());
  


}

void KPostit::print(){

  mykapp->processEvents();
  mykapp->flushX();

  FILE* printpipe;
  
  QString cmd;
  cmd = postitdefaults.printcommand.copy();
  cmd = cmd.sprintf(postitdefaults.printcommand.data(),name.data());
  printf("%s\n",cmd.data());

  printpipe = popen(cmd.data(),"w");
  
  if(printpipe == NULL){
    QString str;
    str.sprintf("Could not pipe the contents of this KNotes into:\n %s",cmd.data());
    QMessageBox::message("Sorry",str.data(),"OK");
    return;
  }

  QTextStream t(printpipe,IO_WriteOnly );

  int line_count = numLines();

  for(int i = 0 ; i < line_count ; i++){
    t << textLine(i) << '\n';
  }

  pclose(printpipe);

}

void KPostit::newKPostit(){

  QString pname;
  bool exists = FALSE;  

  // lets give it the next available name of the for kpostit %d, with %d
  // and integer. If we have more than fifty of those, we give up.

  for (int i = 1; i < 50; i++){

    exists = FALSE;
    pname = "";
    pname.sprintf("knote %d",i);

    for(PostitFilesList.first();
	(PostitFilesList.current() && (!exists));
	  PostitFilesList.next()){

      if ( QString(PostitFilesList.current()) == pname){ // file exist already

	exists = TRUE;
	break;
      }
    }
    if (!exists)
      break;

  }

  if (exists){ // all 50 names are taken
    QMessageBox::message("Sorry","You have exeeded the arbitrary and unjustly set"\
			 "limit of 50 knotes.\n Please complain to the author.",
			 "OK");
    return;
    
  }


  // update all popup menus
  for(PostitList.first();PostitList.current();PostitList.next()){

    // now remove and then reinsert, in a sorted fashion, the popup menu entries
    for(uint i = 0 ; i < PostitFilesList.count();i++){
      PostitList.current()->right_mouse_button->removeItemAt(0);
    }

  }

  PostitFilesList.inSort(pname);

  for(PostitList.first();PostitList.current();PostitList.next()){
    int k = 0;
    for(PostitFilesList.first();PostitFilesList.current();PostitFilesList.next()){
      PostitList.current()->right_mouse_button->insertItem(
	     PostitFilesList.current(),k,k);
      k++;
    }
  }

  KPostit *t = new KPostit (NULL,NULL,PostitFilesList.count() - 1 ,pname);

  t->show ();
  PostitList.append( t );

}

void KPostit::findKPostit(int i){


  if ( i < 0)
    return;

  if ( i >= (int)PostitFilesList.count()){// this is not a kpostit file menue entry
                                          // this should however already been caught
                                          // by the above if (i < 0)
    return;
  }
  
  // unminimize the correponding kpostit if a windget already exists for it

  for(PostitList.first(); PostitList.current() ; PostitList.next()){
    if (PostitList.current()->name == QString(right_mouse_button->text(
					      right_mouse_button->idAt( i)))) {


            kwmcom_send_to_kwm( kwm_un_minimize_window_by_id, 
			  PostitList.current()->winId(),
			   0L, 0L, 0L, 0L);
      PostitList.current()->raise();
      return;
    }
  }

  KPostit *t = new KPostit (NULL,NULL,i,
			    right_mouse_button->text(
			    right_mouse_button->idAt(i)));
  t->show ();
  PostitList.append( t );

}


void KPostit::renameKPostit(){

  QString newName;
  RenameDlg* dlg = new RenameDlg(this,"renamedlg",&newName,&PostitFilesList);
  if(dlg->exec()){

    QString notesfile;
    QString newnotesfile;
    notesfile = getenv("HOME");
    notesfile += "/.kde/knotes/";
    newnotesfile = notesfile.copy();
    notesfile += name;
    newnotesfile += newName;

    if(rename(notesfile.data(),newnotesfile.data())){

    }


    for(PostitFilesList.first();PostitFilesList.current();PostitFilesList.next()){
      if(name == QString(PostitFilesList.current())){
	PostitFilesList.remove(PostitFilesList.at());
	break;
      }
    }

    // update name in timer list if this postit was registered there

    bool have_alarm = FALSE;
    
    mytimer->stop();
    for(KPostit::AlarmList.first();KPostit::AlarmList.current();
	KPostit::AlarmList.next()){

      if (KPostit::AlarmList.current()->name == name){
        have_alarm = TRUE;
	KPostit::AlarmList.current()->name = newName.copy();
      }
    }
    mytimer->start();

    PostitFilesList.inSort(newName);
    name = newName;
    name.detach();

    if( have_alarm )
      setCaption(name + " (A)");
    else
      setCaption(name);

    // remove and reinsert the popup menues in a sorted fashion

    for(PostitList.first();PostitList.current();PostitList.next()){
      for(uint i = 0 ; i < PostitFilesList.count();i++){
	PostitList.current()->right_mouse_button->removeItemAt(0);
      }
    
      int k = 0;
      for(PostitFilesList.first();PostitFilesList.current();PostitFilesList.next()){
	PostitList.current()->right_mouse_button->insertItem(
			     PostitFilesList.current(),k,k);
	k++;	
      }
    }
  }
}

void KPostit::deleteKPostit(){


  if(PostitFilesList.count() == 1){
    QMessageBox::message("Sorry","You can't delete the last KNote.","Ok");
    return;
  }

  int result = QMessageBox::query("Question","Are you sure you want to delete this\n"\
				  "note permanently?","Ok");
  if(!result)
    return;

  QString notesfile;
  notesfile = getenv("HOME");
  notesfile += "/.kde/knotes/";
  notesfile += name;

  if(remove(notesfile.data())){
  }


  for(PostitList.first();PostitList.current();PostitList.next()){
    // remove popup entries
    for(uint i = 0 ; i < PostitFilesList.count();i++){
      PostitList.current()->right_mouse_button->removeItemAt(0);
    }	
  }

  for(PostitFilesList.first();PostitFilesList.current();PostitFilesList.next()){
    if(name == QString(PostitFilesList.current())){
      PostitFilesList.remove(PostitFilesList.at());
      break;
    }
  }
  
  // reinsert PostitFilesList into popus in a sorted fashion
  for(PostitList.first();PostitList.current();PostitList.next()){
    int k = 0;
    for(PostitFilesList.first();PostitFilesList.current();PostitFilesList.next()){
      PostitList.current()->right_mouse_button->insertItem(
					      PostitFilesList.current(),k,k);
      k++;
    }
  }

  mytimer->stop();
  for(KPostit::AlarmList.first();KPostit::AlarmList.current();KPostit::AlarmList.next()){

    if (KPostit::AlarmList.current()->name == name){
      delete KPostit::AlarmList.current();
      KPostit::AlarmList.remove(KPostit::AlarmList.current());
    }
  }
  mytimer->start();

  PostitList.remove( this );
  delete this;    
  if(PostitList.count() == 0){ // this was the last KPostit Object
    // we need to bring back to live at least one, this will not be necessary
    // once we get the kwm communications stuff worked out.
    KPostit* postit;
    postit = new KPostit(NULL,NULL,0,KPostit::PostitFilesList.first());    
    KPostit::PostitList.append( postit ); 
    postit->show();
  }
    
}

bool KPostit::loadnotes(){


  QString notesfile;
  notesfile = getenv("HOME");
  notesfile += "/.kde/knotes/";
  notesfile += name;

  QFile file(notesfile.data());

  
  if( !file.open( IO_ReadOnly )) {
    return FALSE;
  }

  this->setAutoUpdate(FALSE);
  QTextStream t(&file);

  // get the name
  QString name = t.readLine();
  
  // get the geomtery

  QString width = t.readLine();
  QString height = t.readLine();

  QString xpos = t.readLine();
  QString ypos = t.readLine();
  
  //  resize(width.toUInt(),height.toUInt());
  this->setGeometry(xpos.toUInt(),ypos.toUInt(),width.toUInt(),height.toUInt());

  // get the backcolor

  QString red = t.readLine();
  QString green = t.readLine();
  QString blue = t.readLine();

  backcolor.setRgb(red.toUInt(),green.toUInt(),blue.toUInt());

  // get the forecolor

  red = t.readLine();
  green = t.readLine();
  blue = t.readLine();

  forecolor.setRgb(red.toUInt(),green.toUInt(),blue.toUInt());

  // get the font

  QString fontfamily = t.readLine();

  int 	pointsize;
  int 	weight;
  bool 	italic = FALSE;

  QString pointsizestr = t.readLine();
  QString weightstr    = t.readLine();
  QString italicstr    = t.readLine();

  pointsize = pointsizestr.toUInt();
  weight   = weightstr.toUInt();
  int italicint	 = italicstr.toUInt();

  if(italicint == 1)
    italic = TRUE;
  else
    italic = FALSE;

  font = QFont(fontfamily.data(),pointsize,weight,italic);

  
  int int3d;
  QString int3dstr = t.readLine();
  int3d = int3dstr.toUInt();
  if(int3d == 1)
    frame3d = TRUE;
  else
    frame3d = FALSE;

  int indent;
  QString autostr = t.readLine();
  indent = autostr.toUInt();
  if(indent == 1)
    autoIndentMode = TRUE;
  else
    autoIndentMode = FALSE;
  
  
  // get the text body

  while ( !t.eof() ) {
    QString s = t.readLine();
    if(!t.eof())
      this->insertLine( s );
  }

  this->setAutoUpdate(TRUE);
  file.close();
  return TRUE;

}

bool KPostit::insertFile(char* filename){


  QFile file(filename);

  
  if( !file.open( IO_ReadOnly )) {
    QString string;
    string.sprintf("Could not load:\n %s",filename);
    QMessageBox::message("Sorry",string.data(),"OK");
    return FALSE;
  }

  this->setAutoUpdate(FALSE);
  QTextStream t(&file);


  while ( !t.eof() ) {
    QString s = t.readLine();
    if(!t.eof())
      this->insertLine( s );
  }

  this->setAutoUpdate(TRUE);
  file.close();
  this->repaint();
  return TRUE;

}

void KPostit::closeEvent( QCloseEvent * ){

  close();

};

bool KPostit::savenotes(){

  QString notesfile;
  notesfile = getenv("HOME");
  notesfile += "/.kde/knotes/";
  notesfile += name;

  QFile file(notesfile.data());

  if( !file.open( IO_WriteOnly | IO_Truncate )) {
    return FALSE;
  }

  QTextStream t(&file);

  t << name <<'\n';
     
  t << this->width() <<'\n';
  t << this->height() <<'\n';

 
  t << this->frameGeometry().x() <<'\n';
  t << this->frameGeometry().y() <<'\n';
  /*

  t << this->geometry().x() <<'\n';
  t << this->geometry().y() <<'\n';
  */

  t << backcolor.red() <<'\n';
  t << backcolor.green()<< '\n';
  t << backcolor.blue() <<'\n';

  t << forecolor.red()<< '\n';
  t << forecolor.green() <<'\n';
  t << forecolor.blue()<< '\n'; 

  
  t << font.family() <<'\n';
  t << font.pointSize()<< '\n';
  t << font.weight() <<'\n';
  if (font.italic())
    t << 1 <<'\n';
  else
    t <<  0 << '\n';

  if (frame3d)
    t << 1 <<'\n';
  else
    t <<  0 << '\n';

  if (autoIndentMode)
    t << 1 <<'\n';
  else
    t <<  0 << '\n';


  int line_count = numLines();

  for(int i = 0 ; i < line_count ; i++){
    t << textLine(i) << '\n';
  }

  file.close();

  return TRUE;
}

bool KPostit::eventFilter(QObject *o, QEvent *ev){

  static QPoint tmp_point;

  (void) o;

  if(ev->type() != Event_MouseButtonPress) 
    return FALSE;
    
  QMouseEvent *e = (QMouseEvent *)ev;
  
  if(e->button() != RightButton) 
    return FALSE;

  tmp_point = QCursor::pos();
  
  if(right_mouse_button)
    right_mouse_button->popup(tmp_point);

  return TRUE;

}

void KPostit::set3DFrame(){
  
  frame3d = TRUE;
  //  options->changeItem("No Frame",frame3dID);
  options->setItemChecked(frame3dID,TRUE);
  this->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  this->repaint();
}

void KPostit::setNoFrame(){

  frame3d = FALSE;
  //  options->changeItem("3D Frame",frame3dID);
  options->setItemChecked(frame3dID,FALSE);
  this->setFrameStyle(QFrame::NoFrame);
  this->repaint();
}

void KPostit::toggleFrame(){
  if(frame3d)
    setNoFrame();
  else
    set3DFrame();
}

void KPostit::toggleIndentMode(){
  if(autoIndentMode)
    setNoAutoIndent();
  else
    setAutoIndent();
}

void KPostit::setAutoIndent(){
  
  autoIndentMode = TRUE;
  options->setItemChecked(autoIndentID,TRUE);
}

void KPostit::setNoAutoIndent(){

  autoIndentMode = FALSE;
  options->setItemChecked(autoIndentID,FALSE);

}

void KPostit::dummy(){

  
}

void KPostit::defaults()
{

  QTabDialog * tabdialog;
  tabdialog = new QTabDialog(0,"tabdialog",TRUE);
  tabdialog->setCaption( "KNotes Configuraton" );
  tabdialog->resize( 350, 350 );
  tabdialog->setCancelButton();

  QWidget *about = new QWidget(tabdialog,"about");

  QGroupBox *box = new QGroupBox(about,"box");
  QLabel  *label = new QLabel(box,"label");
  box->setGeometry(10,10,320,260);
  box->setTitle("About");
  label->setGeometry(10,25,280,170);
  label->setAlignment( AlignCenter);
  label->setText("KNotes "KNOTES_VERSION"\n"\
                 "Written by Bernd Johannes Wuebben\n"\
		 "wuebben@math.cornell.edu\n"\
		 "(C) 1997\n"\
                 "\n");

  DefStruct newdefstruct;
  newdefstruct.forecolor  = postitdefaults.forecolor;
  newdefstruct.backcolor  = postitdefaults.backcolor;
  newdefstruct.width 	  = postitdefaults.width;
  newdefstruct.height 	  = postitdefaults.height;
  newdefstruct.frame3d 	  = postitdefaults.frame3d;
  newdefstruct.autoindent = postitdefaults.autoindent;
  newdefstruct.font       = postitdefaults.font;
  newdefstruct.mailcommand = postitdefaults.mailcommand;
  newdefstruct.mailcommand.detach();
  newdefstruct.printcommand = postitdefaults.printcommand;
  newdefstruct.printcommand.detach();
  
  ConfigDlg *configdlg;
  configdlg = new ConfigDlg(tabdialog,"configdlg",mykapp,&newdefstruct);

  FontDlg* fontdlg;
  fontdlg = new FontDlg(tabdialog,"fontdlg",mykapp,&newdefstruct);

  tabdialog->addTab(configdlg,"Defaults");
  tabdialog->addTab(fontdlg,"More ...");
  tabdialog->addTab(about,"About");
  
  if(tabdialog->exec() == QDialog::Accepted){


    postitdefaults.forecolor  = newdefstruct.forecolor;
    postitdefaults.backcolor  = newdefstruct.backcolor;
    postitdefaults.width      = newdefstruct.width;
    postitdefaults.height     = newdefstruct.height;
    postitdefaults.frame3d    = newdefstruct.frame3d;
    postitdefaults.autoindent = newdefstruct.autoindent;
    postitdefaults.font       = newdefstruct.font;
    postitdefaults.mailcommand = newdefstruct.mailcommand.copy();
    postitdefaults.printcommand = newdefstruct.printcommand.copy();

  }

}


void KPostit::set_colors(){


  QPalette mypalette = (this->palette()).copy();

  QColorGroup cgrp = mypalette.normal();
  QColorGroup ncgrp(forecolor,cgrp.background(),
		    cgrp.light(),cgrp.dark(),cgrp.mid(),forecolor,backcolor);

  mypalette.setNormal(ncgrp);
  mypalette.setDisabled(ncgrp);
  mypalette.setActive(ncgrp);

  this->setPalette(mypalette);
  this->setBackgroundColor(backcolor);

}


void KPostit::set_foreground_color(){

  QColor color;
  
  color = forecolor;
  
  if(KColorDialog::getColor(color) != QDialog::Accepted)
    return;
  
  forecolor = color;
  set_colors();



}

void KPostit::set_background_color(){

  QColor color;

  color = backcolor;

  if( KColorDialog::getColor(color) != QDialog::Accepted)
    return;
  
  backcolor = color;
  set_colors();
    
}


static void un_minimize(int sig){
  sig = sig;

#ifdef MYDEBUG
  printf("Trying to unminimize:\n Signal received : %d My WinId: %ld \n",
	 sig,window_id);
#endif
  
  kwmcom_send_to_kwm( kwm_un_minimize_window_by_id, 
		      KPostit::PostitList.last()->winId(),
		      0L, 0L, 0L, 0L);
  KPostit::PostitList.last()->raise();
  
  signal(SIGUSR1, un_minimize);
}


void KPostit::slotDropEvent( KDNDDropZone * _dropZone )
{
    QStrList & list = _dropZone->getURLList();
    
    char *s;

    // Load the first file in this window
    if ( (s = list.getFirst()) )
      {
	QString n = s;
	insertNetFile( n.data());
      }			
    
}

void KPostit::insertNetFile( const char *_url)
{
    
  QString string;
  QString netFile = _url;
  netFile.detach();
  KURL u( netFile.data() );

  if ( u.isMalformed() )
    {
	QMessageBox::message ("Sorry", "Malformed URL", "Ok");
	return;
    }

    // Just a usual file ?
    if ( strcmp( u.protocol(), "file" ) == 0 )
    {
      insertFile( u.path());
      return;
    }

    if ( kfm != 0L )
    {
	QMessageBox::message ("Sorry", 
			      "KNotes is already waiting\n"\
			      "for an internet job to finish\n"\
			      "Please wait until it has finished\n"\
			      "Alternatively stop the running one.", "Ok");
	return;
    }
    
    kfm = new KFM;
    if ( !kfm->isOK() )
    {
	QMessageBox::message ("Sorry", "Could not start or find KFM", "Ok");
	delete kfm;
	kfm = 0L;
	return;
    }
    
    tmpFile.sprintf( "file:/tmp/knotes%i", time( 0L ) );
    connect( kfm, SIGNAL( finished() ), this, SLOT( slotKFMFinished() ) );
    kfm->copy( netFile.data(), tmpFile.data() );

}

void KPostit::close(){

  bool result;


  if (!savenotes()){
    result = QMessageBox::query ("Sorry", 
				  "Could not save the KNotes.\n"\
				  "Close anyways?");

	if (!result){
	    return;         // we don't want to exit.
	}
  }

  if ( PostitList.count() > 1 ){ 
    
    PostitList.remove( this );
    delete this;
  }	
  else{
    quit();
  }

}

void KPostit::slotKFMFinished()
{
  
  QString string;

  KURL u( tmpFile.data() );
  insertFile( u.path());

  unlink( tmpFile.data() );
  delete kfm;
  kfm = 0L;
  
}


void findPostitFiles(){

  QString p = getenv( "HOME" );
  QString filesdir = p + "/.kde/knotes/";

  QString alarmdir = p + "/.kde/knotes/xyalarms";

  if ( access( filesdir.data(), F_OK ) ){
    mkdir( filesdir.data(), 0700 );
  }

  if ( access( alarmdir.data(), F_OK ) ){
    mkdir( alarmdir.data(), 0700 );
  }



  QDir d(filesdir);
  d.setSorting( QDir::Name );
  
  const QStrList *list = d.entryList();
  QStrListIterator it( *list );      // create list iterator
  char *fi;                          // pointer for traversing

  while ( (fi=it.current()) ) {           // for each file...
    if (strcmp(".",fi) ==0 || strcmp("..",fi) ==0 ||strcmp("xyalarms",fi) ==0){
      ++it;
      continue;
    }
      
    KPostit::PostitFilesList.inSort(fi);  
    ++it;
  }

}

void alarmConsistencyCheck(){

  for(KPostit::AlarmList.first();KPostit::AlarmList.current();
      KPostit::AlarmList.next()){

    if (KPostit::PostitFilesList.find(KPostit::AlarmList.current()->name.data()) == -1){

      QString str;
      str.sprintf("Found an alarm to which the underlying\n"\
		  "KNotes file:\n"\
		  "%s\n no longer exists.\n\n"\
		  "I will correct the Problem for you."
		  ,KPostit::AlarmList.current()->name.data());

      QMessageBox::message("Inconsistency",str.data(),"OK");

      KPostit::AlarmList.remove(KPostit::AlarmList.current());

    }
  }
}

extern "C" {

static int knotes_x_errhandler( Display *dpy, XErrorEvent *err )
{
    char errstr[256];

    cleanup(0);
    XGetErrorText( dpy, err->error_code, errstr, 256 );
    fatal( "X Error: %s\n  Major opcode:  %d", errstr, err->request_code );
    return 0;
}


static int knotes_xio_errhandler( Display * ){

  cleanup(0);
  /*fatal( "%s: Fatal IO error: client killed", "kppp" );*/
  return 0;
}

} /*extern C*/                                 


int main( int argc, char **argv ) {

  FILE *fp;
  int pid;
  KPostit* postit;

  QString p = getenv( "HOME" );
  QString rcDir = p + "/.kde/config/";
  if ( access( rcDir.data(), F_OK ) )
    mkdir( rcDir.data(), 0740 );

  pidFile = rcDir + "/knotes.pid";

  // if there is a pidFile then this is not the first instance of kpostit

  if ( ( fp = fopen( pidFile, "r" ) ) != NULL )
    {

      fscanf( fp, "%d", &pid);
      printf("pid %d\n",pid);

      // if this fails I assume that the pid file is left over from a bad exit
      // and continue on
      //
      if ( kill( pid, SIGUSR1 ) == 0){ 
	
	// the kpostit is still alive 
	// it could however be zombi or a recycled pid -- use IPC instead.

	exit(0);
      }

      fclose( fp );
    }

  catchSignals();

  fp = fopen( pidFile, "w" );
  fprintf( fp, "%d\n", getpid());
  fclose( fp );

  KApplication a(argc, argv, "knotes");
  mykapp = &a;

  readSettings();

  findPostitFiles();
  readalarms();
  alarmConsistencyCheck();

  if(KPostit::PostitFilesList.count() == 0){
    KPostit::PostitFilesList.append("knote 1");
    postit = new KPostit(NULL,NULL,0,"knote 1");

  }
  else{
    postit = new KPostit(NULL,NULL,0,KPostit::PostitFilesList.first());
  }

  KPostit::PostitList.append( postit ); 

  mytimer = new MyTimer();
  
  postit->show();

  XSetErrorHandler( knotes_x_errhandler );
  XSetIOErrorHandler( knotes_xio_errhandler );

  return a.exec();

  delete mytimer;
}


void readSettings()
{

  QString str;

  KConfig *config = mykapp->getConfig();
  config->setGroup( "Font" );
  postitdefaults.font = config->readFontEntry("Font",
					       &QFont("courier",12));
  config->setGroup("Colors");
  postitdefaults.forecolor = config->readColorEntry("ForeColor",&(QColor)black);
  postitdefaults.backcolor = config->readColorEntry("BackColor",&(QColor)yellow);

  config->setGroup("Geometry");

  postitdefaults.width = config->readNumEntry("width",270);
  postitdefaults.height = config->readNumEntry("height",220);

  config->setGroup("Misc");
  
  postitdefaults.frame3d = (bool) config->readNumEntry("frame3d",(int)FALSE);
  postitdefaults.autoindent = (bool) config->readNumEntry("autoindent",(int)TRUE);

  config->setGroup("Commands");

  postitdefaults.mailcommand = config->readEntry("mailCmd");
  if (postitdefaults.mailcommand.isEmpty())
    postitdefaults.mailcommand = "mail -s\"%s\" %s";

  postitdefaults.printcommand = config->readEntry("printCmd");
  if (postitdefaults.printcommand.isEmpty())
    postitdefaults.printcommand = "a2ps -1 --center-title=\"%s\" "\
      "--underlay=\"KDE\"";
			
}

void writeSettings()
{

  KConfig *config = mykapp->getConfig();		
  
  config->setGroup( "Font" );
  config->writeEntry("Font",postitdefaults.font);
  
  config->setGroup("Colors");
  config->writeEntry("ForeColor",postitdefaults.forecolor);
  config->writeEntry("BackColor",postitdefaults.backcolor);
  
  config->setGroup("Geometry");
  config->writeEntry("width",  postitdefaults.width);
  config->writeEntry("height",  postitdefaults.height);

  config->setGroup("Misc");
  config->writeEntry("frame3d",postitdefaults.frame3d);
  config->writeEntry("autoindent",postitdefaults.autoindent);

  config->setGroup("Commands");
  config->writeEntry("mailCmd",postitdefaults.mailcommand);
  config->writeEntry("printCmd",postitdefaults.printcommand);

  config->sync();

}

static void cleanup( int sig )
{

  (void) sig;
  //  printf("KPostit: Caught signal %d. Tyring to save state.\n",sig);
  remove( pidFile.data() );
  savealarms();
  writeSettings();

  for(KPostit::PostitList.first();KPostit::PostitList.current();
      KPostit::PostitList.next()){

    KPostit::PostitList.current()->savenotes();

  }


  QApplication::exit();
}


// make sure the pid file is cleaned up when exiting unexpectedly.

void catchSignals()
{
	signal(SIGHUP, cleanup);		/* Hangup */
	signal(SIGINT, cleanup);		/* Interrupt */
	signal(SIGTERM, cleanup);		/* Terminate */
//	signal(SIGCHLD, cleanup);

	signal(SIGABRT, cleanup);
	signal(SIGUSR1, un_minimize);
	signal(SIGALRM, cleanup);
	signal(SIGFPE, cleanup);
	signal(SIGILL, cleanup);
//	signal(SIGPIPE, cleanup);
	signal(SIGQUIT, cleanup);
//	signal(SIGSEGV, cleanup);

#ifdef SIGBUS
	signal(SIGBUS, cleanup);
#endif
#ifdef SIGPOLL
	signal(SIGPOLL, cleanup);
#endif
#ifdef SIGSYS
	signal(SIGSYS, cleanup);
#endif
#ifdef SIGTRAP
	signal(SIGTRAP, cleanup);
#endif
#ifdef SIGVTALRM
	signal(SIGVTALRM, cleanup);
#endif
#ifdef SIGXCPU
	signal(SIGXCPU, cleanup);
#endif
#ifdef SIGXFSZ
	signal(SIGXFSZ, cleanup);
#endif
}




