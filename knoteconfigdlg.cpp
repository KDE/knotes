

#include "knoteconfigdlg.h"

#include <kfontdialog.h>
#include <kglobal.h>
#include <kinstance.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

#include <qpixmap.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcolordialog.h>

KNoteConfigDlg::KNoteConfigDlg( KConfig* curr, const QString & title, QWidget* parent, const char* name )
	: KDialogBase( IconList, title, Ok|Apply|Cancel, Cancel, parent, name, true, true )
{
	setIconListAllVisible( true );
	
	_config = curr;
	
	makeDisplayPage();
	makeEditorPage();
	makeActionsPage();
}


KNoteConfigDlg::~KNoteConfigDlg()
{
	_config = NULL;
}
QColor KNoteConfigDlg::getFGColor( KConfig& tmpconfig )
{
	QString str_fgcolor = tmpconfig.entryMap("Display")["fgcolor"];
	kdDebug() << "getFGColor, color is: " << str_fgcolor << endl;
	
	QStringList c = QStringList::split( ' ', str_fgcolor );
	if( c.count() != 3 )
	{
		kdDebug() << "invalid color line" << endl;
		return Qt::white;
	}
	
	int r = c[0].toInt();
	int g = c[1].toInt();
	int b = c[2].toInt();
	
	return QColor( r, g, b );
}

QColor KNoteConfigDlg::getBGColor( KConfig& tmpconfig )
{
	QString str_bgcolor = tmpconfig.entryMap("Display")["bgcolor"];
	kdDebug() << "getBGColor, color is: " << str_bgcolor << endl;
	
	QStringList c = QStringList::split( ' ', str_bgcolor );
	if( c.count() != 3 )
	{
		kdDebug() << "invalid color line" << endl;
		return Qt::white;
	}
	
	int r = c[0].toInt();
	int g = c[1].toInt();
	int b = c[2].toInt();
	
	return QColor( r, g, b );
}

QString KNoteConfigDlg::strColor( const QColor& newc )
{
	QString r = QString::number( newc.red() );
	QString g = QString::number( newc.green() );
	QString b = QString::number( newc.blue() );
	QString color( r + " " + g + " " + b );
	
	return color;
}

QFont KNoteConfigDlg::getFont( KConfig& tmpconfig )
{
	QString strfont = tmpconfig.entryMap("Editor")["font"];
	QString strsize = tmpconfig.entryMap("Editor")["fontsize"];
	QString strstyle = tmpconfig.entryMap("Editor")["fontweight"];
	
	int fontsize  = strsize.toInt();
	int fontweight = strstyle.toInt();
	
	return QFont( strfont, fontsize, fontweight );
}

void KNoteConfigDlg::makeDisplayPage()
{
	QPixmap icon = KGlobal::iconLoader()->loadIcon( "blend", KIcon::Action );
	QFrame* displayPage = addPage( i18n( "Display" ), i18n("Display Settings"), icon );
		
	//set layout- top level is the QVBoxLayout....
	QVBoxLayout* topLevel = new QVBoxLayout( displayPage, 0, spacingHint() );
	QGridLayout* glay     = new QGridLayout( topLevel, 5, 3 );
	
	QLabel* l_fgcolor = new QLabel( i18n("Text Color"), displayPage );
	QLabel* l_bgcolor = new QLabel( i18n("Background Color"), displayPage );
	QLabel* l_width   = new QLabel( i18n("Note Width"), displayPage );
	QLabel* l_height  = new QLabel( i18n("Note Height"), displayPage );

	_fg = getFGColor( *_config );
	_bg = getBGColor( *_config );

	_fgColor = new KColorButton( _fg, displayPage );
	_bgColor = new KColorButton( _bg, displayPage );

	connect( _fgColor, SIGNAL( changed(const QColor&)),
	         this, SLOT( slotFGColor(const QColor&) ) );
	connect( _bgColor, SIGNAL( changed(const QColor&)),
	         this, SLOT( slotBGColor(const QColor&) ) );
	         	
	_width = _config->entryMap("Display")["width"];
	_widthEdit = new KLineEdit( _width, displayPage );	

	_height = _config->entryMap("Display")["height"];
	_heightEdit = new KLineEdit( _height, displayPage );
				
	glay->addWidget( l_fgcolor, 0, 0 );
	glay->addWidget( l_bgcolor, 1, 0 );
	glay->addWidget( _fgColor, 0, 2 );
	glay->addWidget( _bgColor, 1, 2 );
	
	glay->addWidget( l_width, 2, 0 );
	glay->addMultiCellWidget( _widthEdit, 2, 2, 1, 2 );	
	
	glay->addWidget( l_height, 3, 0 );
    glay->addMultiCellWidget( _heightEdit, 3, 3, 1, 2 );
}

void KNoteConfigDlg::makeEditorPage()
{
	QPixmap icon = KGlobal::iconLoader()->loadIcon( "editcopy", KIcon::Action );
	QFrame* editorPage = addPage( i18n( "Editor" ), i18n("Editor Settings"), icon );
	
	//set layout- top level is the QVBoxLayout....
	QVBoxLayout* topLevel = new QVBoxLayout( editorPage, 0, spacingHint() );
	QGridLayout* glay     = new QGridLayout( topLevel, 3, 3 );
	
	QLabel* l_tabsize = new QLabel( i18n( "Tab Size" ), editorPage );
	glay->addWidget( l_tabsize, 0, 0 );
	
	_tab = _config->entryMap("Editor")["tabsize"];
	_tabEdit = new KLineEdit( _tab, editorPage );
	glay->addMultiCellWidget( _tabEdit, 0, 0, 1, 2 );
	
	QString val = _config->entryMap("Editor")["autoindent"];
	_autoIndentSwitch = new QCheckBox( i18n("Auto Indent"), editorPage );
	if( val == "yes" )
		_autoIndentSwitch->setChecked( true );
	else _autoIndentSwitch->setChecked( false );
	glay->addMultiCellWidget( _autoIndentSwitch, 1, 1, 0, 2, AlignCenter );
	
	_font = new QPushButton( editorPage );
	QFont currfont = getFont( *_config );
	_font->setFont( currfont );
	_font->setText( i18n( "Current Font: Click to Change" ) );
	glay->addMultiCellWidget( _font, 2, 2, 0, 2, AlignCenter );
	connect( _font, SIGNAL(clicked()), this, SLOT(slotChangeFont()) );
}

void KNoteConfigDlg::makeActionsPage()
{
	QPixmap icon = KGlobal::iconLoader()->loadIcon( "run", KIcon::Action );
	QFrame* actionsPage = addPage( i18n( "Actions" ), i18n("Action Settings"), icon );
	
	//set layout- top level is the QVBoxLayout....
	QVBoxLayout* topLevel = new QVBoxLayout( actionsPage, 0, spacingHint() );
	QGridLayout* glay     = new QGridLayout( topLevel, 3, 3 );
	
	QLabel* l_mail = new QLabel( i18n("Mail Action"), actionsPage );
	_mail = _config->entryMap("Actions")["mail"];
	_mailEdit = new KLineEdit( _mail, actionsPage );
	glay->addWidget( l_mail, 0, 0 );
	glay->addMultiCellWidget( _mailEdit, 0, 0, 1, 2 );

	
	//printing not implemented yet
	//QLabel* l_print = new QLabel( i18n("Print Action"), actionsPage );
	//glay->addWidget( l_print, 2, 0 );
}

void KNoteConfigDlg::storeSettings()
{
	//store Display settings
	QString fgc = strColor( _fg );
	QString bgc = strColor( _bg );
	_height = _heightEdit->text();
	_width  = _widthEdit->text();

	_config->setGroup( "Display" );
	_config->writeEntry( "fgcolor", fgc );
	_config->writeEntry( "bgcolor", bgc );
	_config->writeEntry( "height", _height );
	_config->writeEntry( "width", _width );
	
	//store Editor settings
	QFont currfont = _font->font();
	QString fontname = currfont.family();
	QString fontsize = QString::number( currfont.pointSize() );
	QString fontweight = QString::number( currfont.weight() );
	bool autoindent = _autoIndentSwitch->isChecked();
	QString strbool;
	if( autoindent ) strbool = "yes";
	else             strbool = "no";
	_tab = _tabEdit->text();
	
	_config->setGroup( "Editor" );
	_config->writeEntry( "font", fontname );
	_config->writeEntry( "fontsize", fontsize );
	_config->writeEntry( "fontweight", fontweight );
	_config->writeEntry( "tabsize", _tab );
	_config->writeEntry( "autoindent", strbool );

	//store Action Settings ** TODO **
	
	_config->sync();	
}

void KNoteConfigDlg::slotOk()
{
	//get data from ls
	storeSettings();
	emit updateConfig();
	
	close();
}

void KNoteConfigDlg::slotApply()
{
	storeSettings();
	emit updateConfig();
}

void KNoteConfigDlg::slotChangeFont()
{
	QFont newfont;
	QString example = i18n("Example Text");
	KFontDialog::getFontAndText( newfont, example );
	
	_font->setFont( newfont );
}

void KNoteConfigDlg::slotFGColor( const QColor& c )
{
	_fg = c; //QColorDialog::getColor( _fg, this );
	_fgColor->setBackgroundColor( _fg );
}

void KNoteConfigDlg::slotBGColor( const QColor& c )
{
	_bg = c; //QColorDialog::getColor( _bg, this );
	_bgColor->setBackgroundColor( _fg );
	
}
#include "knoteconfigdlg.moc"
