

#ifndef KNOTECONFIGDLG_H
#define KNOTECONFIGDLG_H

#include <kdialogbase.h>
#include <kconfig.h>
#include <klineedit.h>
#include <kconfig.h>
#include <kaction.h>
#include <kcolorbtn.h>


class KNoteConfigDlg : public KDialogBase  {
   Q_OBJECT
public:
	KNoteConfigDlg( KConfig* curr, QString title, QWidget* parent=0, const char* name=0 );
	~KNoteConfigDlg();
	
	void makeDisplayPage();
	void makeEditorPage();
	void makeActionsPage();
	
	static QColor getFGColor( KConfig* tmpconfig );
	static QColor getBGColor( KConfig* tmpconfig );
	static QString strColor( const QColor& newc );
		
protected:
	KConfig*      _config;
	
	KColorButton* _fgColor;
	KColorButton* _bgColor;
	KLineEdit*    _widthEdit;
	KLineEdit*    _heightEdit;
	KLineEdit*    _tabEdit;
	KRadioAction* _autoIndentSwitch;
	KLineEdit*    _mailEdit;
	KLineEdit*    _dateEdit;
	KLineEdit*    _printEdit;
		
protected slots:
	virtual void slotOk();
	virtual void slotApply();
	
	void slotChangeFont();
	void slotFGColor( const QColor& newColor );
	void slotBGColor( const QColor& newColor );
	
signals:
	void updateConfig();
	
private:  //store data internally, and then apply to _config on ok or apply
	QColor  _fg;
	QColor  _bg;
	QString _width;
	QString _height;
	QString _tab;
	bool    _autoindent;
	QString _mail;
	QString _date;
	
	void storeSettings();
};

#endif
