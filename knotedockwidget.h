

#ifndef KNOTEDOCKWIDGET_H
#define KNOTEDOCKWIDGET_H


#include <kdockwindow.h>
#include <kapp.h>
#include <qwidget.h>



class KNoteDockWidget : public KDockWindow  {
   Q_OBJECT
public: 
	KNoteDockWidget( KApplication* parent, const char* name=0 );
	~KNoteDockWidget();
};

#endif
