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
#include <kseparator.h>
#include <kwin.h>

#include "knoteconfigdlg.h"
#include "version.h"

#include <netwm.h>

KNoteConfigDlg::KNoteConfigDlg( const QString& configfile, const QString& title,
            bool global, QWidget* parent, const char* name )
    : KDialogBase( IconList, title, Ok|Apply|Cancel, Ok, parent, name, true, true )
{
    setIconListAllVisible( true );
    m_config = new KConfig( configfile, false, false );
    m_global = global;

    makeDisplayPage();
    makeEditorPage();
    makeActionsPage();
}

KNoteConfigDlg::~KNoteConfigDlg()
{
    // just delete m_config, all others have parent widgets
    // and will get destroyed by the parent
    delete m_config;
}

void KNoteConfigDlg::makeDisplayPage()
{
    m_config->setGroup( "Display" );

    QPixmap icon = KGlobal::iconLoader()->loadIcon( "knotes", KIcon::Toolbar, KIcon::SizeMedium );
    QFrame* displayPage = addPage( i18n("Display"), i18n("Display Settings"), icon );

    // set layout - top level is the QVBoxLayout...
    QVBoxLayout* topLevel = new QVBoxLayout( displayPage, 0, spacingHint() );
    QGridLayout* glay     = new QGridLayout( topLevel, 6, 3 );
    glay->setColStretch( 1, 1 );

    QLabel* l_fgcolor = new QLabel( i18n("&Text color:"), displayPage );
    QLabel* l_bgcolor = new QLabel( i18n("&Background color:"), displayPage );

    QColor fgcolor = m_config->readColorEntry( "fgcolor", &(Qt::black) );
    QColor bgcolor = m_config->readColorEntry( "bgcolor", &(Qt::yellow) );
    m_fgColor = new KColorButton( fgcolor, displayPage );
    m_bgColor = new KColorButton( bgcolor, displayPage );
    connect( m_fgColor, SIGNAL(changed(const QColor&)),
            this, SLOT(slotFGColor(const QColor&)) );
    connect( m_bgColor, SIGNAL(changed(const QColor&)),
            this, SLOT(slotBGColor(const QColor&)) );

    l_fgcolor->setBuddy( m_fgColor );
    l_bgcolor->setBuddy( m_bgColor );

    m_config->setGroup( "WindowDisplay" );
    ulong state = m_global ? m_config->readUnsignedLongNumEntry( "state", NET::SkipTaskbar )
                           : KWin::info( parentWidget()->winId() ).state;

    m_skipTaskbarSwitch = new QCheckBox( i18n("&Show Note in Taskbar"), displayPage );
    m_skipTaskbarSwitch->setChecked( !(state & NET::SkipTaskbar) );

    KSeparator *sep = new KSeparator( Horizontal, displayPage );

    glay->addWidget( l_fgcolor, 0, 0 );
    glay->addWidget( l_bgcolor, 1, 0 );
    glay->addWidget( m_fgColor, 0, 2 );
    glay->addWidget( m_bgColor, 1, 2 );
    glay->addMultiCellWidget( sep, 4, 4, 0, 2 );
    glay->addWidget( m_skipTaskbarSwitch, 5, 0 );

    if ( m_global )
    {
        QLabel* l_width  = new QLabel( i18n("Default &width:"), displayPage );
        QLabel* l_height = new QLabel( i18n("Default &height:"), displayPage );

        m_config->setGroup( "Display" );
        uint width = m_config->readUnsignedNumEntry( "width", 200 );
        uint height = m_config->readUnsignedNumEntry( "height", 200 );

        m_widthEdit = new KIntNumInput( width, displayPage );
        m_widthEdit->setRange( 100, 2000, 10, false );
        m_heightEdit = new KIntNumInput( height, displayPage );
        m_heightEdit->setRange( 100, 2000, 10, false );

        l_width->setBuddy( m_widthEdit );
        l_height->setBuddy( m_heightEdit );

        glay->addWidget( l_width, 2, 0 );
        glay->addWidget( m_widthEdit, 2, 2 );
        glay->addWidget( l_height, 3, 0 );
        glay->addWidget( m_heightEdit, 3, 2 );
    }
}

void KNoteConfigDlg::makeEditorPage()
{
    m_config->setGroup( "Editor" );

    QPixmap icon = KGlobal::iconLoader()->loadIcon( "edit", KIcon::Toolbar, KIcon::SizeMedium );
    QFrame* editorPage = addPage( i18n( "Editor" ), i18n("Editor Settings"), icon );

    // set layout - top level is the QVBoxLayout...
    QVBoxLayout* topLevel = new QVBoxLayout( editorPage, 0, spacingHint() );
    QGridLayout* glay     = new QGridLayout( topLevel, 4, 3 );
    glay->setColStretch( 1, 1 );

    QLabel* l_tabsize = new QLabel( i18n( "&Tab size:" ), editorPage );
    glay->addWidget( l_tabsize, 0, 0 );

    int tabsize = m_config->readUnsignedNumEntry( "tabsize", 4 );
    m_tabEdit = new KIntNumInput( tabsize, editorPage );
    m_tabEdit->setRange( 0, 20, 1, false );
    glay->addWidget( m_tabEdit, 0, 2 );
    l_tabsize->setBuddy( m_tabEdit );

    bool check_val = m_config->readBoolEntry( "autoindent", true );
    m_autoIndentSwitch = new QCheckBox( i18n("Auto &indent"), editorPage );
    m_autoIndentSwitch->setChecked( check_val );
    glay->addWidget( m_autoIndentSwitch, 1, 0, AlignCenter );

    check_val = m_config->readBoolEntry( "richtext", false );
    m_richTextSwitch = new QCheckBox( i18n("&Rich text"), editorPage );
    m_richTextSwitch->setChecked( check_val );
    glay->addWidget( m_richTextSwitch, 1, 2, AlignCenter );

    m_titleFont = new QPushButton( editorPage );
    QFont def_font(KGlobalSettings::generalFont());
    QFont currfont = m_config->readFontEntry( "titlefont", &def_font );
    m_titleFont->setFont( currfont );
    m_titleFont->setText( i18n( "Title Font: Click to Change..." ) );
    glay->addMultiCellWidget( m_titleFont, 2, 2, 0, 2 );
    connect( m_titleFont, SIGNAL(clicked()), this, SLOT(slotChangeTitleFont()) );

    m_textFont = new QPushButton( editorPage );
    currfont = m_config->readFontEntry( "font", &def_font );
    m_textFont->setFont( currfont );
    m_textFont->setText( i18n( "Text Font: Click to Change..." ) );
    glay->addMultiCellWidget( m_textFont, 3, 3, 0, 2 );
    connect( m_textFont, SIGNAL(clicked()), this, SLOT(slotChangeTextFont()) );
}

void KNoteConfigDlg::makeActionsPage()
{
    m_config->setGroup( "Actions" );

    QPixmap icon = KGlobal::iconLoader()->loadIcon( "misc", KIcon::Toolbar, KIcon::SizeMedium );
    QFrame* actionsPage = addPage( i18n( "Actions" ), i18n("Action Settings"), icon );

    // set layout - top level is the QVBoxLayout...
    QVBoxLayout* topLevel = new QVBoxLayout( actionsPage, 0, spacingHint() );
    QGridLayout* glay     = new QGridLayout( topLevel, 2, 2 );
    glay->setColStretch( 1, 1 );

    QLabel* l_mail = new QLabel( i18n("&Mail action:"), actionsPage );
    QString mailstr = m_config->readPathEntry( "mail", "kmail --msg %f" );
    m_mailEdit = new KLineEdit( mailstr, actionsPage );
    l_mail->setBuddy( m_mailEdit );
    glay->addWidget( l_mail, 0, 0 );
    glay->addWidget( m_mailEdit, 0, 1 );
}

void KNoteConfigDlg::storeSettings()
{
    // store General settings
    m_config->setGroup( "General" );

    m_config->writeEntry( "version", KNOTES_VERSION );

    // store Display settings
    m_config->setGroup( "Display" );

    m_config->writeEntry( "fgcolor", m_fgColor->color() );
    m_config->writeEntry( "bgcolor", m_bgColor->color() );

    if ( m_global )
    {
        m_config->writeEntry( "height", m_heightEdit->value() );
        m_config->writeEntry( "width", m_widthEdit->value() );

        m_config->setGroup( "WindowDisplay" );
        m_config->writeEntry( "state", !m_skipTaskbarSwitch->isChecked() ? NET::SkipTaskbar : 0 );
    }

    // store Editor settings
    m_config->setGroup( "Editor" );

    m_config->writeEntry( "titlefont", m_titleFont->font() );
    m_config->writeEntry( "font", m_textFont->font() );
    m_config->writeEntry( "autoindent", m_autoIndentSwitch->isChecked() );
    m_config->writeEntry( "richtext", m_richTextSwitch->isChecked() );
    m_config->writeEntry( "tabsize", m_tabEdit->value() );

    // store Action Settings
    m_config->setGroup( "Actions" );

    m_config->writePathEntry( "mail", m_mailEdit->text() );

    // TODO: use kconf_update?
    m_config->deleteEntry( "print" );

    m_config->sync();
}

void KNoteConfigDlg::slotOk()
{
    // get data from ls
    storeSettings();
    emit updateConfig();
    emit skipTaskbar( !m_skipTaskbarSwitch->isChecked() );

    accept();
}

void KNoteConfigDlg::slotApply()
{
    storeSettings();
    emit updateConfig();
    emit skipTaskbar( !m_skipTaskbarSwitch->isChecked() );
}

void KNoteConfigDlg::slotChangeTitleFont()
{
    QFont newfont = m_titleFont->font();
    KFontDialog::getFont( newfont );

    m_titleFont->setFont( newfont );
}

void KNoteConfigDlg::slotChangeTextFont()
{
    QFont newfont = m_textFont->font();
    KFontDialog::getFont( newfont );

    m_textFont->setFont( newfont );
}

void KNoteConfigDlg::slotFGColor( const QColor& c )
{
    m_fgColor->setBackgroundColor( c );
}

void KNoteConfigDlg::slotBGColor( const QColor& c )
{
    m_bgColor->setBackgroundColor( c );
}


#include "knoteconfigdlg.moc"
