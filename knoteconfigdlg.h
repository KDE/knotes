

#ifndef KNOTECONFIGDLG_H
#define KNOTECONFIGDLG_H

#include <kdialogbase.h>
#include <kconfig.h>


class KNoteConfigDlg : public KDialogBase  {
   Q_OBJECT
public:
	KNoteConfigDlg( KConfig* curr, QWidget* parent=0, const char* name=0 );
	~KNoteConfigDlg();
};

#endif
