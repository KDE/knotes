

#include "knoteconfigdlg.h"

KNoteConfigDlg::KNoteConfigDlg( KConfig* curr, QWidget* parent, const char* name )
	: KDialogBase(parent,name)
{
	resize( 400, 400 );
}


KNoteConfigDlg::~KNoteConfigDlg()
{
}
