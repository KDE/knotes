

#ifndef KNOTE_H
#define KNOTE_H

#include "knoteedit.h"
#include "knotebutton.h"

#include <kconfig.h>
#include <kpopupmenu.h>
#include <qframe.h>
#include <qlabel.h>
#include <qdir.h>


class KNote : public QFrame  {
   Q_OBJECT
public: 
	KNote( KConfig* config, QWidget* parent=0, const char* name=0 );
	~KNote();
	
	void save();
	QString getName()
	{
	    if( m_label )
	         return m_label->text();
	    else return QString::null;
	}

private:
	void applyConfig();

public slots:
	//menu slots
	void slotMail   ( int id=0 );
	void slotPrint  ( int id=0 );
	void slotRename ( int id=0 );
	void slotInsDate( int id=0 );
	void slotConfig ( int id=0 );
	void slotKill   ( int id=0 );
	void slotClose  ( );
	
signals:
	void sigClosed( QString& );
	void sigRenamed( QString&, QString& );
	void sigNewNote( int );
	
protected:
	void resizeEvent( QResizeEvent* );
	bool eventFilter( QObject* o, QEvent* e );
			
	KNoteEdit*   m_editor;
    QString      m_title;
    KConfig*     m_config;
    KNoteButton* m_button;
    QLabel*      m_label;
    KPopupMenu*  m_menu;

    bool         m_dragging;
    QPoint       m_pointerOffset;
    int          m_id;
    QDir*        m_notedir;
};


#endif
