/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2003, The KNotes Developers

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*******************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qcheckbox.h>

#include <kconfig.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <knuminput.h>
#include <klineedit.h>
#include <kfontdialog.h>
#include <kiconloader.h>

#include "knoteconfigdlg.h"
#include "version.h"

KNoteConfigDlg::KNoteConfigDlg( const QString& configfile, const QString& title,
            bool global, QWidget* parent, const char* name )
    : KDialogBase( IconList, title, Ok|Apply|Cancel, Ok, parent, name, true, true )
{
    setIconListAllVisible( true );
    _config = new KConfig( configfile, false, false );
    _global = global;

    makeDisplayPage();
    makeEditorPage();
    makeActionsPage();
}

KNoteConfigDlg::~KNoteConfigDlg()
{
    // just delete _config, all others have parent widgets
    // and will get destroyed by the parent
    delete _config;
}

void KNoteConfigDlg::makeDisplayPage()
{
    _config->setGroup( "Display" );

//    QPixmap icon = KGlobal::iconLoader()->loadIcon( "appearance", KIcon::Action, KIcon::SizeMedium );
    QPixmap icon = KGlobal::iconLoader()->loadIcon( "appearance", KIcon::Toolbar, KIcon::SizeMedium );
    QFrame* displayPage = addPage( i18n( "Display" ), i18n("Display Settings"), icon );

    // set layout - top level is the QVBoxLayout...
    QVBoxLayout* topLevel = new QVBoxLayout( displayPage, 0, spacingHint() );
    QGridLayout* glay     = new QGridLayout( topLevel, 5, 3 );
    glay->setColStretch( 1, 1 );

    QLabel* l_fgcolor = new QLabel( i18n("&Text color:"), displayPage );
    QLabel* l_bgcolor = new QLabel( i18n("&Background color:"), displayPage );

    QColor fgcolor = _config->readColorEntry( "fgcolor", &(Qt::black) );
    QColor bgcolor = _config->readColorEntry( "bgcolor", &(Qt::yellow) );
    _fgColor = new KColorButton( fgcolor, displayPage );
    _bgColor = new KColorButton( bgcolor, displayPage );
    connect( _fgColor, SIGNAL( changed(const QColor&)),
            this, SLOT( slotFGColor(const QColor&) ) );
    connect( _bgColor, SIGNAL( changed(const QColor&)),
            this, SLOT( slotBGColor(const QColor&) ) );

    l_fgcolor->setBuddy( _fgColor );
    l_bgcolor->setBuddy( _bgColor );

    glay->addWidget( l_fgcolor, 0, 0 );
    glay->addWidget( l_bgcolor, 1, 0 );
    glay->addWidget( _fgColor, 0, 2 );
    glay->addWidget( _bgColor, 1, 2 );

    if ( _global )
    {
        QLabel* l_width  = new QLabel( i18n("Default &width:"), displayPage );
        QLabel* l_height = new QLabel( i18n("Default &height:"), displayPage );

        uint width = _config->readUnsignedNumEntry( "width", 200 );
        uint height = _config->readUnsignedNumEntry( "height", 200 );

        _widthEdit = new KIntNumInput( width, displayPage );
        _widthEdit->setRange( 100, 2000, 10, false );
        _heightEdit = new KIntNumInput( height, displayPage );
        _heightEdit->setRange( 100, 2000, 10, false );

        l_width->setBuddy( _widthEdit );
        l_height->setBuddy( _heightEdit );

        glay->addWidget( l_width, 2, 0 );
        glay->addWidget( _widthEdit, 2, 2 );
        glay->addWidget( l_height, 3, 0 );
        glay->addWidget( _heightEdit, 3, 2 );
    }
}

void KNoteConfigDlg::makeEditorPage()
{
    _config->setGroup( "Editor" );

//    QPixmap icon = KGlobal::iconLoader()->loadIcon( "edit", KIcon::Action, KIcon::SizeMedium );
    QPixmap icon = KGlobal::iconLoader()->loadIcon( "edit", KIcon::Toolbar, KIcon::SizeMedium );
    QFrame* editorPage = addPage( i18n( "Editor" ), i18n("Editor Settings"), icon );

    // set layout - top level is the QVBoxLayout...
    QVBoxLayout* topLevel = new QVBoxLayout( editorPage, 0, spacingHint() );
    QGridLayout* glay     = new QGridLayout( topLevel, 4, 3 );
    glay->setColStretch( 1, 1 );

    QLabel* l_tabsize = new QLabel( i18n( "&Tab size:" ), editorPage );
    glay->addWidget( l_tabsize, 0, 0 );

    int tabsize = _config->readUnsignedNumEntry( "tabsize", 4 );
    _tabEdit = new KIntNumInput( tabsize, editorPage );
    _tabEdit->setRange( 0, 20, 1, false );
    glay->addWidget( _tabEdit, 0, 2 );
    l_tabsize->setBuddy( _tabEdit );

    bool check_val = _config->readBoolEntry( "autoindent", true );
    _autoIndentSwitch = new QCheckBox( i18n("Auto &indent"), editorPage );
    _autoIndentSwitch->setChecked( check_val );
    glay->addWidget( _autoIndentSwitch, 1, 0, AlignCenter );

    check_val = _config->readBoolEntry( "richtext", false );
    _richTextSwitch = new QCheckBox( i18n("&Rich text"), editorPage );
    _richTextSwitch->setChecked( check_val );
    glay->addWidget( _richTextSwitch, 1, 2, AlignCenter );

    _titleFont = new QPushButton( editorPage );
    QFont def_font(KGlobalSettings::generalFont());
    QFont currfont = _config->readFontEntry( "titlefont", &def_font );
    _titleFont->setFont( currfont );
    _titleFont->setText( i18n( "Title Font: Click to Change..." ) );
    glay->addMultiCellWidget( _titleFont, 2, 2, 0, 2 );
    connect( _titleFont, SIGNAL(clicked()), this, SLOT(slotChangeTitleFont()) );

    _textFont = new QPushButton( editorPage );
    currfont = _config->readFontEntry( "font", &def_font );
    _textFont->setFont( currfont );
    _textFont->setText( i18n( "Text Font: Click to Change..." ) );
    glay->addMultiCellWidget( _textFont, 3, 3, 0, 2 );
    connect( _textFont, SIGNAL(clicked()), this, SLOT(slotChangeTextFont()) );
}

void KNoteConfigDlg::makeActionsPage()
{
    _config->setGroup( "Actions" );

//    QPixmap icon = KGlobal::iconLoader()->loadIcon( "misc", KIcon::Action, KIcon::SizeMedium );
    QPixmap icon = KGlobal::iconLoader()->loadIcon( "misc", KIcon::Toolbar, KIcon::SizeMedium );
    QFrame* actionsPage = addPage( i18n( "Actions" ), i18n("Action Settings"), icon );

    // set layout - top level is the QVBoxLayout...
    QVBoxLayout* topLevel = new QVBoxLayout( actionsPage, 0, spacingHint() );
    QGridLayout* glay     = new QGridLayout( topLevel, 2, 2 );
    glay->setColStretch( 1, 1 );

    QLabel* l_mail = new QLabel( i18n("&Mail action:"), actionsPage );
    QString mailstr = _config->readEntry( "mail", "kmail --msg %f" );
    _mailEdit = new KLineEdit( mailstr, actionsPage );
    l_mail->setBuddy( _mailEdit );
    glay->addWidget( l_mail, 0, 0 );
    glay->addWidget( _mailEdit, 0, 1 );
}

void KNoteConfigDlg::storeSettings()
{
    // store General settings
    _config->setGroup( "General" );

    _config->writeEntry( "version", KNOTES_VERSION );

    // store Display settings
    _config->setGroup( "Display" );

    _config->writeEntry( "fgcolor", _fgColor->color() );
    _config->writeEntry( "bgcolor", _bgColor->color() );

    if ( _global )
    {
        _config->writeEntry( "height", _heightEdit->value() );
        _config->writeEntry( "width", _widthEdit->value() );
    }

    // store Editor settings
    _config->setGroup( "Editor" );

    _config->writeEntry( "titlefont", _titleFont->font() );
    _config->writeEntry( "font", _textFont->font() );
    _config->writeEntry( "autoindent", _autoIndentSwitch->isChecked() );
    _config->writeEntry( "richtext", _richTextSwitch->isChecked() );
    _config->writeEntry( "tabsize", _tabEdit->value() );

    // store Action Settings
    _config->setGroup( "Actions" );

    _config->writeEntry( "mail", _mailEdit->text() );

    // TODO: use kconf_update?
    _config->deleteEntry( "print" );

    _config->sync();
}

void KNoteConfigDlg::slotOk()
{
    // get data from ls
    storeSettings();
    emit updateConfig();

    accept();
}

void KNoteConfigDlg::slotApply()
{
    storeSettings();
    emit updateConfig();
}

void KNoteConfigDlg::slotChangeTitleFont()
{
    QFont newfont = _titleFont->font();
    KFontDialog::getFont( newfont );

    _titleFont->setFont( newfont );
}

void KNoteConfigDlg::slotChangeTextFont()
{
    QFont newfont = _textFont->font();
    KFontDialog::getFont( newfont );

    _textFont->setFont( newfont );
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
