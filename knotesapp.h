

#ifndef KNOTESAPP_H
#define KNOTESAPP_H

#include <kuniqueapp.h>
#include <kdockwindow.h>

#include "knote.h"
#include "knotedockwidget.h"


class KNotesApp : public KDockWindow  {
   Q_OBJECT
public:
	KNotesApp();
	~KNotesApp();
	
protected:
	QMap< QString, KNote* >  m_NoteList;
	KNoteDockWidget*         m_DockWidget;
		
public slots:
	void slotNewNote    ( int id=0 );  //needed to connect to menu
	void slotNoteClosed ( QString& name );
	void slotPreferences( int id=0 );
	void slotNoteRenamed( QString& oldname, QString& newname );
	
private:
	bool first_instance;
};

#endif
