

#include "knotedockwidget.h"

#include <kpopupmenu.h>
#include <klocale.h>

KNoteDockWidget::KNoteDockWidget( KApplication* parent, const char* name )
	: KDockWindow( NULL, name )
{
	KPopupMenu* menu = contextMenu();
		
	//new note option
	menu->insertItem( i18n("New Note"), parent, SLOT(slotNewNote(int)) );
		
	//configure option
	menu->insertItem( i18n("Preferences..."), parent, SLOT(slotPreferences(int)) );
}


KNoteDockWidget::~KNoteDockWidget()
{
}



