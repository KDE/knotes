
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


#include <time.h>
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
#include <kwm.h>

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
bool    saved_already_for_session_management = false;

extern bool     savealarms();
extern bool 	readalarms();

#include <dirent.h>
#include <sys/stat.h>

// Torben
void testDir( const char *_name )
{
    DIR *dp;
    QString c = getenv( "HOME" );
    c += _name;
    dp = opendir( c.data() );
    if ( dp == NULL )
	::mkdir( c.data(), S_IRWXU );
    else
	closedir( dp );
}


KPostitMultilineEdit::KPostitMultilineEdit(QWidget *parent, const char *myname)
  : QMultiLineEdit(parent, myname){
}

void KPostitMultilineEdit::keyPressEvent(QKeyEvent *e){

  if (e->key() == Key_Tab)
    insertChar('\t');

  if(e->key() == Key_Return || e->key() == Key_Enter){
    
    mynewLine();
    return;
  }

  QMultiLineEdit::keyPressEvent(e);

}

void KPostitMultilineEdit::mynewLine(){

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

QString KPostitMultilineEdit::prefixString(QString string){
  
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


KPostit::KPostit(QWidget *parent, const char *myname,int  _number, QString pname)
  : QFrame(parent, myname){

    KWM::setDecoration(winId(), 2); 
    KWM::setWmCommand(winId(), "");
    XSetTransientForHint( qt_xdisplay(), winId(), (Window)(-1));
    KWM::setIcon(winId(), kapp->getIcon());
    KWM::setMiniIcon(winId(), kapp->getMiniIcon());

    label = new QLabel(this);
    label->setText("Hallo");
    label->setAlignment( AlignHCenter);
    label->installEventFilter(this);
    dragging = false;

    edit = new KPostitMultilineEdit(this);
    edit->setGeometry(0,30,200,100);
    edit->installEventFilter(this);
    edit->setFocus();

    hidden = false;
    number = _number; 	// index in popup. Not used anymore, but I'll leave it in
                        // the structure for now.
    name = pname;
    name.detach(); 	// name of postit and name on the popup

    kfm = 0L;

    //set the defaults
    forecolor 		= postitdefaults.forecolor;
    backcolor 		= postitdefaults.backcolor;
    edit->autoIndentMode 	= postitdefaults.autoindent;
    font 		= postitdefaults.font;
    frame3d 		= postitdefaults.frame3d;

    resize(postitdefaults.width,postitdefaults.height + 30);
    edit->resize(postitdefaults.width,postitdefaults.height);

    loadnotes();

    colors =  	new QPopupMenu ();

    colors->insertItem(klocale->translate("Text Color"),this, SLOT(set_foreground_color()));
    colors->insertItem(klocale->translate("Background Color"),this, SLOT(set_background_color()));

    operations =  	new QPopupMenu ();
    operations->insertItem(klocale->translate("Clear"),this, SLOT(clear_text()));

    operations->insertSeparator();
    operations->insertItem (klocale->translate("New Note"), this, 	
				    SLOT(newKPostit()));
    operations->insertItem (klocale->translate("Delete Note"), this, 	
				    SLOT(deleteKPostit()));
    operations->insertItem (klocale->translate("Rename Note"),this,SLOT(renameKPostit()));
    operations->insertSeparator();
    operations->insertItem (klocale->translate("Alarm ..."), this, 	
				    SLOT(setAlarm()));
    operations->insertSeparator();
    operations->insertItem (klocale->translate("Calendar"), this, 	
				    SLOT(insertCalendar()));
    operations->insertItem (klocale->translate("Mail Note ..."), this, 	
				    SLOT(mail()));
    operations->insertItem (klocale->translate("Print Note"), this, 	
				    SLOT(print()));

    options = new QPopupMenu();
    options->setCheckable(TRUE);
    options->setFont(QFont("Helvetica",12));
    frame3dID = options->insertItem(klocale->translate("3D Frame"),this, SLOT(toggleFrame()));
    edit->autoIndentID = options->insertItem(klocale->translate("Auto Indent"),this, 
				       SLOT(toggleIndentMode()));
    options->insertItem(klocale->translate("Font"),this, SLOT(selectFont()));
    options->insertItem(klocale->translate("Colors"),colors);
    options->insertSeparator();
    options->insertItem(klocale->translate("Change Defaults ..."),this, SLOT(defaults()));


    operations->insertSeparator();
    operations->insertItem (klocale->translate("Help"),this,SLOT(help()));
    operations->insertSeparator();
    operations->insertItem ( klocale->translate("Quit"), this,
 				    SLOT(quit()));
    operations->insertSeparator();
    operations->insertItem (klocale->translate("Options"),options);

    right_mouse_button = new QPopupMenu;

    for ( uint i = 0; i < PostitFilesList.count(); i++){
      
      int k = right_mouse_button->insertItem (PostitFilesList.at(i));
      k = k;
    }

    connect( right_mouse_button, SIGNAL(activated( int )), 
	     SLOT(findKPostit(int)) );


    right_mouse_button->insertSeparator();
    right_mouse_button->insertItem(klocale->translate("Hide Note"), this,
		       SLOT(hideKPostit()));
    right_mouse_button->insertItem (klocale->translate("Insert Date"), this, 	
				    SLOT(insertDate()));
  right_mouse_button->insertSeparator();
    right_mouse_button->insertItem (klocale->translate("Operations"),operations);

    right_mouse_button->insertSeparator();

    sticky_id = right_mouse_button->insertItem("", this, SLOT(toggleSticky())); 
    
    desktops = new QPopupMenu;
    connect( desktops, SIGNAL(activated( int )), 
	     SLOT(toDesktop(int)) );

    right_mouse_button->insertItem(KWM::getToDesktopString(), 
				   desktops);

    //    right_mouse_button->insertSeparator();

//     right_mouse_button->insertItem (klocale->translate("Quit"), this,
// 				    SLOT(quit()));

    //    right_mouse_button->insertItem(klocale->translate("Delete Note"), this,
    //	       SLOT(deleteKPostit()));



    installEventFilter( this );     

    window_id = winId();

    KDNDDropZone * dropZone = new KDNDDropZone( this , DndURL);
    connect( dropZone, SIGNAL( dropAction( KDNDDropZone *) ), 
	   this, SLOT( slotDropEvent( KDNDDropZone *) ) );


    if(!frame3d){
      setNoFrame();
      options->changeItem(klocale->translate("3D Frame"),frame3dID);
    }
    else{
      set3DFrame();
      options->changeItem(klocale->translate("No Frame"),frame3dID);
    }

    if(!edit->autoIndentMode){
      setNoAutoIndent();
      options->setItemChecked(edit->autoIndentID,FALSE);
    }
    else{
      setAutoIndent();
      options->setItemChecked(edit->autoIndentID,TRUE);
    }

    set_colors();
    edit->setFont(font);
    //    label->setFont(font);

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
    label->setText(caption());
    

    connect(mykapp,SIGNAL(saveYourself()),this,SLOT(wm_saveyourself()));
    
}

void KPostit::toggleshow(){
  
}

void KPostit::wm_saveyourself(){

  if(!saved_already_for_session_management){
  
    remove( pidFile.data() );
    savealarms();
    writeSettings();
    
    for(KPostit::PostitList.first();KPostit::PostitList.current();
	KPostit::PostitList.next()){

      KPostit::PostitList.current()->savenotes();

    }
    saved_already_for_session_management = true;
  }
}


void KPostit::clear_text(){

  edit->clear();	
}

void KPostit::selectFont(){
 
  QFont myfont = QWidget::font();
  KFontDialog::getFont(myfont);
  edit->setFont(myfont);
  //  label->setFont(myfont);
  font = myfont;
  
}

void KPostit::quit(){

  for(PostitList.first();PostitList.current();PostitList.next()){

    if (!PostitList.current()->savenotes()){
      int result = QMessageBox::query (klocale->translate("Sorry"), 
				       klocale->translate("Could not save a KNote.\n"\
				       "Quit anyways?"));

      if (!result){
	return;         
      }
    }
  }	

  remove( pidFile.data() );
  writeSettings();
  if(!savealarms()){
    QString str;
    str.sprintf(klocale->translate("Could not save KNote Alarms\n"));
    QMessageBox::message(klocale->translate("Sorry"),str.data(),
                         klocale->translate("OK"));
  }
  QApplication::exit();

}


void KPostit::help(){

  mykapp->invokeHTMLHelp("","");

}

void KPostit::insertDate(){
  
  int line, column;

  QString string;
  QDateTime dt = QDateTime::currentDateTime();
  string = dt.toString();

  edit->getCursorPosition(&line,&column);
  edit->insertAt(string,line,column);
 
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
    str.sprintf(klocale->translate("Could not pipe the contents of this KNote into:\n %s"),cmd.data());
    QMessageBox::message(klocale->translate("Sorry"),str.data(),"OK");
    return;
  }

  QTextStream t(mailpipe,IO_WriteOnly );

  int line_count = edit->numLines();

  for(int i = 0 ; i < line_count ; i++){
    t << edit->textLine(i) << '\n';
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
      edit->insertLine( s );
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
  entry->name = name.copy();
  entry->dt = qdt;

  AlarmList.append(entry);
  QString str;
  str.sprintf("%s (A)",name.data());
  setCaption(str.data());
  label->setText(caption());

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
    str.sprintf(klocale->translate("Could not pipe the contents of this KNotes into:\n %s"),cmd.data());
    QMessageBox::message(klocale->translate("Sorry"),str.data(),klocale->translate("OK"));
    return;
  }

  QTextStream t(printpipe,IO_WriteOnly );

  int line_count = edit->numLines();

  for(int i = 0 ; i < line_count ; i++){
    t << edit->textLine(i) << '\n';
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
    QMessageBox::message(klocale->translate("Sorry"),klocale->translate("You have exeeded the arbitrary and unjustly set"\
			 "limit of 50 knotes.\n Please complain to the author."),
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
  
  // activate the correponding kpostit if a windget already exists for it

  for(PostitList.first(); PostitList.current() ; PostitList.next()){
    if (PostitList.current()->name == QString(right_mouse_button->text(
					      right_mouse_button->idAt( i)))) {
      if( PostitList.current()->hidden == true){
	PostitList.current()->hidden = false;
	if(propertystring != (QString) "")
	  KWM::setProperties(PostitList.current()->winId(),
			     PostitList.current()->propertystring);

      }
	KWM::activate(PostitList.current()->winId());
      PostitList.current()->show();
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
    notesfile += "/.kde/share/apps/knotes/notes";
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
    label->setText(caption());

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

void KPostit::hideKPostit(){

    hidden = true;
    propertystring = KWM::getProperties(winId());
    this->hide();

}


void KPostit::deleteKPostit(){


  int result = QMessageBox::warning(this, klocale->translate("Question"),
		klocale->translate("Are you sure you want to delete this\n"\
				  "note permanently?"), 
				    QMessageBox::Yes, 
				    QMessageBox::Cancel);
  if(result == QMessageBox::Cancel)
    return;

  QString notesfile;
  notesfile = getenv("HOME");
  notesfile += "/.kde/share/apps/knotes/notes/";
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
}

bool KPostit::loadnotes(){


  QString notesfile;
  notesfile = getenv("HOME");
  notesfile += "/.kde/share/apps/knotes/notes/";
  notesfile += name;

  QFile file(notesfile.data());

  
  if( !file.open( IO_ReadOnly )) {
    return FALSE;
  }

  edit->setAutoUpdate(FALSE);
  QTextStream t(&file);

  // get the name
  QString name = t.readLine();
  
  // get the geomtery

  QString geom = t.readLine();
  propertystring = geom;

  setGeometry(KWM::setProperties(winId(), geom));

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
    edit->autoIndentMode = TRUE;
  else
    edit->autoIndentMode = FALSE;
  
  QString hiddenstring = t.readLine();
  int hiddenint = hiddenstring.toUInt();
  if(hiddenint == 1)
    hidden = true;
  else
    hidden = false;

  // get the text body

  while ( !t.eof() ) {
    QString s = t.readLine();
    if(!t.eof())
      edit->insertLine( s );
  }

  edit->setAutoUpdate(TRUE);
  file.close();
  return TRUE;

}

bool KPostit::insertFile(char* filename){


  QFile file(filename);

  
  if( !file.open( IO_ReadOnly )) {
    QString string;
    string.sprintf(klocale->translate("Could not load:\n %s"),filename);
    QMessageBox::message(klocale->translate("Sorry"),string.data(),klocale->translate("OK"));
    return FALSE;
  }

  edit->setAutoUpdate(FALSE);
  QTextStream t(&file);


  while ( !t.eof() ) {
    QString s = t.readLine();
    if(!t.eof())
      edit->insertLine( s );
  }

  edit->setAutoUpdate(TRUE);
  file.close();
  edit->repaint();
  return TRUE;

}



void KPostit::resizeEvent( QResizeEvent * ){
  label->adjustSize();
  label->setGeometry(0,0,width(),label->height());
  edit->setGeometry(0, label->height(), width(), height()-label->height());
}

void KPostit::closeEvent( QCloseEvent * ){
  deleteKPostit();
};

bool KPostit::savenotes(){

  QString notesfile;
  notesfile = getenv("HOME");
  notesfile += "/.kde/share/apps/knotes/notes/";
  notesfile += name;

  QFile file(notesfile.data());

  if( !file.open( IO_WriteOnly | IO_Truncate )) {
    return FALSE;
  }

  QTextStream t(&file);

  t << name <<'\n';
     
  t << KWM::getProperties(winId()) <<'\n';
 

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

  if (edit->autoIndentMode)
    t << 1 <<'\n';
  else
    t <<  0 << '\n';

  if (hidden)
    t << 1 <<'\n';
  else
    t <<  0 << '\n';

  

  int line_count = edit->numLines();

  for(int i = 0 ; i < line_count ; i++){
    t << edit->textLine(i) << '\n';
  }

  file.close();

  return TRUE;
}

bool KPostit::eventFilter(QObject *o, QEvent *ev){

  static QPoint tmp_point;

  QMouseEvent *e = (QMouseEvent *)ev;


  if (o == label){
    if (ev->type() == Event_MouseButtonRelease){
      if (e->button() == LeftButton){
        dragging = false;
        label->releaseMouse();
	raise();
      }
      if (e->button() == MidButton)
	lower();
      return TRUE;
    }

    if (ev->type() == Event_MouseButtonPress
	&& e->button() == LeftButton){
      pointerOffset = e->pos();
      label->grabMouse(sizeAllCursor);
      return TRUE;
    }
    if (ev->type() == Event_MouseMove
	&& label == mouseGrabber()){
      if (dragging)
	move(QCursor::pos()-pointerOffset);
      else {
	dragging = (
	  (e->pos().x() - pointerOffset.x())
	  *
	  (e->pos().x() - pointerOffset.x())
	  +
	  (e->pos().y() - pointerOffset.y())
	    *
	  (e->pos().y() - pointerOffset.y())
	  >= 9);
      }
      return TRUE;
    }
    
    if (ev->type() == Event_MouseMove)
      return TRUE;
  }
  
  (void) o;

  if(ev->type() != Event_MouseButtonPress) 
    return FALSE;
    
  if(e->button() != RightButton)
    return FALSE;

  tmp_point = QCursor::pos();
  
  if(right_mouse_button){
    desktops->clear();
    int i;
    int n = KWM::numberOfDesktops();
    for (i=1; i <= n; i++){
      QString b = "&";
      b.append(KWM::getDesktopName(i));
      desktops->insertItem(b, i);
    }
    desktops->setItemChecked(KWM::currentDesktop(), TRUE);
    if (KWM::isSticky(winId()))
      right_mouse_button->changeItem(KWM::getUnStickyString(), sticky_id);
    else
      right_mouse_button->changeItem(KWM::getStickyString(), sticky_id);
    right_mouse_button->popup(tmp_point);
  }

  return TRUE;

}

void KPostit::set3DFrame(){
  
  frame3d = TRUE;
  //  options->changeItem("No Frame",frame3dID);
  options->setItemChecked(frame3dID,TRUE);
  edit->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  edit->repaint();
}

void KPostit::setNoFrame(){

  frame3d = FALSE;
  //  options->changeItem("3D Frame",frame3dID);
  options->setItemChecked(frame3dID,FALSE);
  edit->setFrameStyle(QFrame::NoFrame);
  edit->repaint();
}

void KPostit::toggleFrame(){
  if(frame3d)
    setNoFrame();
  else
    set3DFrame();
}


void KPostit::dummy(){

  
}

void KPostit::defaults()
{

  QTabDialog * tabdialog;
  tabdialog = new QTabDialog(0,"tabdialog",TRUE);
  tabdialog->setCaption( klocale->translate("KNotes Configuraton") );
  tabdialog->resize( 350, 350 );
  tabdialog->setCancelButton();

  QWidget *about = new QWidget(tabdialog,"about");

  QGroupBox *box = new QGroupBox(about,"box");
  QLabel  *label = new QLabel(box,"label");
  box->setGeometry(10,10,315,260);

  box->setTitle("About");


  label->setGeometry(140,60,160,170);

  QString labelstring = "KNotes "KNOTES_VERSION"\n"\
    "Bernd Johannes Wuebben\n"\
    "wuebben@math.cornell.edu\n"\
    "wuebben@kde.org\n"\
    "Copyright (C) 1997\n"\
    "\n\n";


  label->setAlignment(AlignLeft|WordBreak|ExpandTabs);
  label->setText(labelstring.data());
  
  QString pixdir = mykapp->kdedir() + QString("/share/apps/knotes/pics/");  


  QPixmap pm((pixdir + "knoteslogo.xpm").data());
  QLabel *logo = new QLabel(box);
  logo->setPixmap(pm);
  logo->setGeometry(30, 50, pm.width(), pm.height());

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

  tabdialog->addTab(configdlg,klocale->translate("Defaults"));
  tabdialog->addTab(fontdlg,klocale->translate("More ..."));
  tabdialog->addTab(about,klocale->translate("About"));
  
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


  QPalette mypalette = (edit->palette()).copy();

  QColorGroup cgrp = mypalette.normal();
  QColorGroup ncgrp(forecolor,cgrp.background(),
		    cgrp.light(),cgrp.dark(),cgrp.mid(),forecolor,backcolor);

  mypalette.setNormal(ncgrp);
  mypalette.setDisabled(ncgrp);
  mypalette.setActive(ncgrp);

  edit->setPalette(mypalette);
  edit->setBackgroundColor(backcolor);
  label->setPalette(mypalette);
  label->setBackgroundColor(backcolor.dark(120));

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


static void siguser1(int sig){
  sig = sig;

  if (KPostit::PostitList.count() == 0){
    KPostit::PostitFilesList.append("knote 1");
    KPostit* postit = new KPostit(NULL,NULL,0,"knote 1");
    KPostit::PostitList.append(postit); 
    postit->show();
  }
  else {
      KPostit::PostitList.last()->show();
      KWM::activate(KPostit::PostitList.last()->winId());
      //    KPostit::PostitList.last()->newKPostit();
  }
  
  signal(SIGUSR1, siguser1);
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
	QMessageBox::message (klocale->translate("Sorry"), 
		klocale->translate("Malformed URL"), 
		klocale->translate("Ok"));
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
	QMessageBox::message (klocale->translate("Sorry"), 
		      klocale->translate("KNotes is already waiting\n"\
			      "for an internet job to finish\n"\
			      "Please wait until it has finished\n"\
			      "Alternatively stop the running one."), 
			klocale->translate("Ok"));
	return;
    }
    
    kfm = new KFM;
    if ( !kfm->isOK() )
    {
	QMessageBox::message (klocale->translate("Sorry"), 
			klocale->translate("Could not start or find KFM"), 
			klocale->translate("Ok"));
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
    result = QMessageBox::query (klocale->translate("Sorry"), 
				  klocale->translate("Could not save the KNotes.\n"\
				  "Close anyways?"));

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



void KPostit::toggleIndentMode(){
  if(edit->autoIndentMode)
    setNoAutoIndent();
  else
    setAutoIndent();
}

void KPostit::toDesktop(int d){
  KWM::moveToDesktop(winId(), d);
}

void KPostit::toggleSticky(){
  KWM::setSticky(winId(), !KWM::isSticky(winId()));
}

void KPostit::setAutoIndent(){
  
  edit->autoIndentMode = TRUE;
  options->setItemChecked(edit->autoIndentID,TRUE);
}

void KPostit::setNoAutoIndent(){

  edit->autoIndentMode = FALSE;
  options->setItemChecked(edit->autoIndentID,FALSE);

}


void findPostitFiles(){

  QString p = getenv( "HOME" );
  QString filesdir = p + "/.kde/share/apps/knotes/notes/";

  QString alarmdir = p + "/.kde/share/apps/knotes/xyalarms";

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
      str.sprintf(klocale->translate("Found an alarm to which the underlying\n"\
		  "KNotes file:\n"\
		  "%s\n no longer exists.\n\n"\
		  "I will correct the Problem for you.")
		  ,KPostit::AlarmList.current()->name.data());

      QMessageBox::message(klocale->translate("Inconsistency"),str.data(),
			klocale->translate("OK"));

      KPostit::AlarmList.remove(KPostit::AlarmList.current());

    }
  }
}

int main( int argc, char **argv ) {

  FILE *fp;
  int pid;
  KPostit* postit;
  
  // Torben
  testDir( "/.kde" );
  testDir( "/.kde/share" );      
  testDir( "/.kde/share/config" );
  testDir( "/.kde/share/apps" );
  testDir( "/.kde/share/apps/knotes" );
  testDir( "/.kde/share/apps/knotes/notes" );
  testDir( "/.kde/share/apps/knotes/xyalarms" );

  QString p = getenv( "HOME" );
  QString rcDir = p + "/.kde/share/apps/knotes";
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

  mytimer = new MyTimer();

  bool restoring = false;

  if (QString("-restore") == (QString)argv[1]){
    restoring = true;
  }


  if(KPostit::PostitFilesList.count() == 0 ){
    KPostit::PostitFilesList.append("knote 1");
  }

  unsigned int i;
  bool one_is_visible = false;

  for (i=0; i<KPostit::PostitFilesList.count(); i++){
    postit = new KPostit(NULL,NULL,0,KPostit::PostitFilesList.at(i));
    KPostit::PostitList.append(postit); 
    if(!postit->hidden){
      postit->show();
      one_is_visible = true;
    }
  }

  if(!one_is_visible && !restoring){

    KPostit::PostitList.last()->show(); 
  }

  // manual session management (knotes alredy stores everything)
  kapp->setTopWidget(new QWidget);
  kapp->enableSessionManagement(true);
  kapp->setWmCommand("knotes -restore");

  return a.exec();

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
	signal(SIGUSR1, siguser1);
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




