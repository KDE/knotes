

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

KNoteConfigDlg::KNoteConfigDlg( KConfig* curr, const QString & title,
                                QWidget* parent, const char* name )
        : KDialogBase( IconList, title, Ok|Apply|Cancel, Ok,
                       parent, name, true, true )
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

void KNoteConfigDlg::makeDisplayPage()
{
    _config->setGroup( "Display" );

    QPixmap icon = KGlobal::iconLoader()->loadIcon( "blend", KIcon::Action );
    QFrame* displayPage = addPage( i18n( "Display" ), i18n("Display Settings"), icon );

    //set layout- top level is the QVBoxLayout....
    QVBoxLayout* topLevel = new QVBoxLayout( displayPage, 0, spacingHint() );
    QGridLayout* glay     = new QGridLayout( topLevel, 5, 3 );

    QLabel* l_fgcolor = new QLabel( i18n("Text Color"), displayPage );
    QLabel* l_bgcolor = new QLabel( i18n("Background Color"), displayPage );
    QLabel* l_width   = new QLabel( i18n("Note Width"), displayPage );
    QLabel* l_height  = new QLabel( i18n("Note Height"), displayPage );

    QColor fgcolor = _config->readColorEntry( "fgcolor", &(Qt::black) );
    QColor bgcolor = _config->readColorEntry( "bgcolor", &(Qt::yellow) );
    _fgColor = new KColorButton( fgcolor, displayPage );
    _bgColor = new KColorButton( bgcolor, displayPage );
    connect( _fgColor, SIGNAL( changed(const QColor&)),
            this, SLOT( slotFGColor(const QColor&) ) );
    connect( _bgColor, SIGNAL( changed(const QColor&)),
            this, SLOT( slotBGColor(const QColor&) ) );

    uint width = _config->readUnsignedNumEntry( "width", 100 );
    uint height = _config->readUnsignedNumEntry( "height", 100 );
    _widthEdit = new KIntNumInput( width, displayPage );
    _widthEdit->setRange( 100, 2000, 10, false );
    _heightEdit = new KIntNumInput( height, displayPage );
    _heightEdit->setRange( 100, 2000, 10, false );

    glay->addWidget( l_fgcolor, 0, 0 );
    glay->addWidget( l_bgcolor, 1, 0 );
    glay->addWidget( _fgColor, 0, 2 );
    glay->addWidget( _bgColor, 1, 2 );

    glay->addWidget( l_width, 2, 0 );
    glay->addWidget( _widthEdit, 2, 2 );

    glay->addWidget( l_height, 3, 0 );
    glay->addWidget( _heightEdit, 3, 2 );
}

void KNoteConfigDlg::makeEditorPage()
{
    _config->setGroup( "Editor" );

    QPixmap icon = KGlobal::iconLoader()->loadIcon( "editcopy", KIcon::Action );
    QFrame* editorPage = addPage( i18n( "Editor" ), i18n("Editor Settings"), icon );

    //set layout- top level is the QVBoxLayout....
    QVBoxLayout* topLevel = new QVBoxLayout( editorPage, 0, spacingHint() );
    QGridLayout* glay     = new QGridLayout( topLevel, 3, 3 );

    QLabel* l_tabsize = new QLabel( i18n( "Tab Size" ), editorPage );
    glay->addWidget( l_tabsize, 0, 0 );

    int tabsize = _config->readUnsignedNumEntry( "tabsize", 4 );
    _tabEdit = new KIntNumInput( tabsize, editorPage );
    _tabEdit->setRange( 0, 20, 1, false );
    glay->addWidget( _tabEdit, 0, 2 );

    bool check_val = _config->readBoolEntry( "autoindent", true );
    _autoIndentSwitch = new QCheckBox( i18n("Auto Indent"), editorPage );
    _autoIndentSwitch->setChecked( check_val );
    glay->addMultiCellWidget( _autoIndentSwitch, 1, 1, 0, 2, AlignCenter );

    _font = new QPushButton( editorPage );
    QFont def_font( "helvetica" );
    QFont currfont = _config->readFontEntry( "font", &def_font );
    _font->setFont( currfont );
    _font->setText( i18n( "Current Font: Click to Change" ) );
    glay->addMultiCellWidget( _font, 2, 2, 0, 2, AlignCenter );
    connect( _font, SIGNAL(clicked()), this, SLOT(slotChangeFont()) );
}

void KNoteConfigDlg::makeActionsPage()
{
    _config->setGroup( "Actions" );

    QPixmap icon = KGlobal::iconLoader()->loadIcon( "run", KIcon::Action );
    QFrame* actionsPage = addPage( i18n( "Actions" ), i18n("Action Settings"), icon );

    //set layout- top level is the QVBoxLayout....
    QVBoxLayout* topLevel = new QVBoxLayout( actionsPage, 0, spacingHint() );
    QGridLayout* glay     = new QGridLayout( topLevel, 3, 3 );

    QLabel* l_mail = new QLabel( i18n("Mail Action"), actionsPage );
    QString mailstr = _config->readEntry( "mail", "kmail --msg %f" );
    _mailEdit = new KLineEdit( mailstr, actionsPage );
    glay->addWidget( l_mail, 0, 0 );
    glay->addMultiCellWidget( _mailEdit, 0, 0, 1, 2 );


    //printing not implemented yet
    QLabel* l_print = new QLabel( i18n("Print"), actionsPage );
    QString printstr = _config->readEntry( "print", "a2ps -P %p -1 --center-title=%t --underlay=KDE %f" );
    _printEdit = new KLineEdit( printstr, actionsPage );
    glay->addWidget( l_print, 1, 0 );
    glay->addMultiCellWidget( _printEdit, 1, 1, 1, 2 );
}

void KNoteConfigDlg::storeSettings()
{
    //store Display settings
    _config->setGroup( "Display" );

    _config->writeEntry( "fgcolor", _fgColor->color() );
    _config->writeEntry( "bgcolor", _bgColor->color() );

    uint testval;

    testval = _heightEdit->value();
    _config->writeEntry( "height", testval );

    testval = _widthEdit->value();
    _config->writeEntry( "width", testval );

    //store Editor settings
    _config->setGroup( "Editor" );

    _config->writeEntry( "font", _font->font() );

    bool autoindent = _autoIndentSwitch->isChecked();
    _config->writeEntry( "autoindent", autoindent );

    testval = _tabEdit->value();
    _config->writeEntry( "tabsize", testval );


    //store Action Settings ** TODO **
    _config->setGroup( "Actions" );
    _config->writeEntry( "mail", _mailEdit->text() );
    _config->writeEntry( "print", _printEdit->text() );

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
    _fgColor->setBackgroundColor( c );
}

void KNoteConfigDlg::slotBGColor( const QColor& c )
{
    _bgColor->setBackgroundColor( c );
}


#include "knoteconfigdlg.moc"
