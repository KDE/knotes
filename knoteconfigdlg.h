

#ifndef KNOTECONFIGDLG_H
#define KNOTECONFIGDLG_H

#include <kdialogbase.h>
#include <kconfig.h>
#include <klineedit.h>
#include <kconfig.h>
#include <kcolorbtn.h>
#include <kcolordialog.h>
#include <knuminput.h>

#include <qcheckbox.h>
#include <qfont.h>

class KNoteConfigDlg : public KDialogBase  {
   Q_OBJECT
public:
    KNoteConfigDlg( KConfig* curr, const QString & title,
                    QWidget* parent=0, const char* name=0 );
    ~KNoteConfigDlg();

    void makeDisplayPage();
    void makeEditorPage();
    void makeActionsPage();

protected:
    KConfig*      _config;

    KColorButton* _fgColor;
    KColorButton* _bgColor;

    KIntNumInput* _widthEdit;
    KIntNumInput* _heightEdit;
    KIntNumInput* _tabEdit;

    QCheckBox*    _autoIndentSwitch;
    KLineEdit*    _mailEdit;
    KLineEdit*    _printEdit;
    QPushButton*  _font;

protected slots:
    virtual void slotOk();
    virtual void slotApply();

    void slotChangeFont();
    void slotFGColor( const QColor& );
    void slotBGColor( const QColor& );

signals:
    void updateConfig();

private:
    void storeSettings();

};

#endif
