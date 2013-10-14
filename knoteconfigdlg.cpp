/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2005, The KNotes Developers

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*******************************************************************/


#include "knoteconfigdlg.h"
#include "knote.h"
#include "knotesglobalconfig.h"

#include "kdepim-version.h"

#include <kapplication.h>
#include <kcolorbutton.h>
#include <kconfig.h>
#include <kfontrequester.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <knuminput.h>
#include <kwindowsystem.h>

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>


KNoteSimpleConfigDlg::KNoteSimpleConfigDlg( KNoteConfig *config, const QString &title,
                                QWidget *parent, const QString &name )
  : KConfigDialog( parent, name, config )
{
  setFaceType( KPageDialog::List );
  setButtons( Default | Ok | Apply | Cancel  );
  setDefaultButton( Ok );

  setCaption( title );
#ifdef Q_WS_X11
  KWindowSystem::setIcons( winId(),
                           qApp->windowIcon().pixmap(
                             IconSize( KIconLoader::Desktop ),
                             IconSize( KIconLoader::Desktop ) ),
                           qApp->windowIcon().pixmap(
                             IconSize( KIconLoader::Small ),
                             IconSize( KIconLoader::Small ) ) );
#endif
  showButtonSeparator( true );

  addPage( new KNoteDisplayConfigWidget( false ), i18n( "Display" ), QLatin1String("knotes"),
             i18n( "Display Settings" ) );
  addPage( new KNoteEditorConfigWidget( false ), i18n( "Editor" ), QLatin1String("accessories-text-editor"),
             i18n( "Editor Settings" ) );
  config->setVersion( QLatin1String(KDEPIM_VERSION) );
}


void KNoteSimpleConfigDlg::slotUpdateCaption(const QString & name)
{
    setCaption( name );
}

KNoteConfigDlg::KNoteConfigDlg( const QString &title,
                                QWidget *parent )
  : KCMultiDialog( parent )
{
  setFaceType( KPageDialog::List );
  setButtons( Default | Ok | Cancel );
  setDefaultButton( Ok );

  setCaption( title );
#ifdef Q_WS_X11
  KWindowSystem::setIcons( winId(),
                           qApp->windowIcon().pixmap(
                             IconSize( KIconLoader::Desktop ),
                             IconSize( KIconLoader::Desktop ) ),
                           qApp->windowIcon().pixmap(
                             IconSize( KIconLoader::Small ),
                             IconSize( KIconLoader::Small ) ) );
#endif
  showButtonSeparator( true );

  addModule( QLatin1String("knote_config_display") );
  addModule( QLatin1String("knote_config_editor") );
  addModule( QLatin1String("knote_config_action") );
  addModule( QLatin1String("knote_config_network") );

  KNotesGlobalConfig::self()->setVersion( QLatin1String(KDEPIM_VERSION) );
  connect( this, SIGNAL(okClicked()), SLOT(slotOk()) );
}

KNoteConfigDlg::~KNoteConfigDlg()
{
}

void KNoteConfigDlg::slotOk() {
  KNotesGlobalConfig::self()->writeConfig();
  emit configWrote();
}


extern "C"
{
  KDE_EXPORT KCModule *create_knote_config_display( QWidget *parent )
  {
      KComponentData instance( "kcmnote_config_display" );
      return new KNoteDisplayConfig( instance, parent );
  }
}

extern "C"
{
  KDE_EXPORT KCModule *create_knote_config_editor( QWidget *parent )
  {
      KComponentData instance( "kcmnote_config_editor" );
      return new KNoteEditorConfig( instance, parent );
  }
}

extern "C"
{
  KDE_EXPORT KCModule *create_knote_config_action( QWidget *parent )
  {
      KComponentData instance( "kcmnote_config_action" );
      return new KNoteActionConfig( instance, parent );
  }
}

extern "C"
{
  KDE_EXPORT KCModule *create_knote_config_network( QWidget *parent )
  {
      KComponentData instance( "kcmnote_config_network" );
      return new KNoteNetworkConfig( instance, parent );
  }
}

KNoteDisplayConfigWidget::KNoteDisplayConfigWidget( bool defaults )
    :QWidget( 0 )
{
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setMargin( defaults ? KDialog::marginHint() : 0 );

    QLabel *label_FgColor = new QLabel( i18n( "&Text color:" ),this );
    label_FgColor->setObjectName( QLatin1String("label_FgColor") );
    layout->addWidget( label_FgColor, 0, 0 );

    KColorButton *kcfg_FgColor = new KColorButton( this );
    kcfg_FgColor->setObjectName( QLatin1String("kcfg_FgColor") );
    label_FgColor->setBuddy( kcfg_FgColor );
    layout->addWidget( kcfg_FgColor, 0, 1 );

    QLabel *label_BgColor = new QLabel( i18n( "&Background color:" ),
                                        this );
    label_BgColor->setObjectName( QLatin1String("label_BgColor") );
    layout->addWidget( label_BgColor, 1, 0 );

    KColorButton *kcfg_BgColor = new KColorButton( this );
    kcfg_BgColor->setObjectName( QLatin1String("kcfg_BgColor") );
    label_BgColor->setBuddy( kcfg_BgColor );
    layout->addWidget( kcfg_BgColor, 1, 1 );

    QCheckBox *kcfg_ShowInTaskbar =
        new QCheckBox( i18n( "&Show note in taskbar" ), this );
    kcfg_ShowInTaskbar->setObjectName( QLatin1String("kcfg_ShowInTaskbar") );
#ifdef Q_WS_X11
    QCheckBox *kcfg_RememberDesktop =
        new QCheckBox( i18n( "&Remember desktop" ), this );
    kcfg_RememberDesktop->setObjectName( QLatin1String("kcfg_RememberDesktop") );
#endif
    if ( defaults ) {
        QLabel *label_Width = new QLabel( i18n( "Default &width:" ), this );

        layout->addWidget( label_Width, 2, 0 );

        KIntNumInput *kcfg_Width = new KIntNumInput( this );
        kcfg_Width->setObjectName( QLatin1String("kcfg_Width") );
        label_Width->setBuddy( kcfg_Width );
        kcfg_Width->setRange( 50, 2000, 10 );
        kcfg_Width->setSliderEnabled( false );
        layout->addWidget( kcfg_Width, 2, 1 );

        QLabel *label_Height = new QLabel( i18n( "Default &height:" ),
                                           this );
        layout->addWidget( label_Height, 3, 0 );

        KIntNumInput *kcfg_Height = new KIntNumInput( this );
        kcfg_Height->setObjectName( QLatin1String("kcfg_Height") );
        kcfg_Height->setRange( 50, 2000, 10 );
        kcfg_Height->setSliderEnabled( false );
        label_Height->setBuddy( kcfg_Height );
        layout->addWidget( kcfg_Height, 3, 1 );

        layout->addWidget( kcfg_ShowInTaskbar, 4, 0 );
#ifdef Q_WS_X11
        layout->addWidget( kcfg_RememberDesktop, 5, 0 );
#endif
    } else {
        layout->addWidget( kcfg_ShowInTaskbar, 2, 0 );
#ifdef Q_WS_X11
        layout->addWidget( kcfg_RememberDesktop, 3, 0 );
#endif
    }
    layout->setRowStretch(4,1);
}

KNoteDisplayConfig::KNoteDisplayConfig( const KComponentData &inst, QWidget *parent )
    :KCModule( inst, parent )
{
    QVBoxLayout *lay = new QVBoxLayout( this );
    QWidget * w =  new KNoteDisplayConfigWidget( true );
    lay->addWidget( w );
    lay->addStretch();
    addConfig( KNotesGlobalConfig::self(), w );
    load();
}

void KNoteDisplayConfig::load()
{
    KCModule::load();
}

void KNoteDisplayConfig::save()
{
    KCModule::save();
}

KNoteEditorConfigWidget::KNoteEditorConfigWidget( bool defaults )
    :QWidget( 0 )
{
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setMargin( defaults ? KDialog::marginHint() : 0 );

    QLabel *label_TabSize = new QLabel( i18n( "&Tab size:" ), this );
    layout->addWidget( label_TabSize, 0, 0, 1, 2 );

    KIntNumInput *kcfg_TabSize = new KIntNumInput( this );
    kcfg_TabSize->setObjectName( QLatin1String("kcfg_TabSize") );
    kcfg_TabSize->setRange( 0, 40 );
    kcfg_TabSize->setSliderEnabled( false );
    label_TabSize->setBuddy( kcfg_TabSize );
    layout->addWidget( kcfg_TabSize, 0, 2 );

    QCheckBox *kcfg_AutoIndent = new QCheckBox( i18n( "Auto &indent" ),
                                                this );
    kcfg_AutoIndent->setObjectName( QLatin1String("kcfg_AutoIndent") );
    layout->addWidget( kcfg_AutoIndent, 1, 0, 1, 2 );

    QCheckBox *kcfg_RichText = new QCheckBox( i18n( "&Rich text" ), this );
    kcfg_RichText->setObjectName( QLatin1String("kcfg_RichText") );
    layout->addWidget( kcfg_RichText, 1, 2 );

    QLabel *label_Font = new QLabel( i18n( "Text font:" ), this );
    layout->addWidget( label_Font, 3, 0 );

    KFontRequester *kcfg_Font = new KFontRequester( this );
    kcfg_Font->setObjectName( QLatin1String("kcfg_Font") );
    kcfg_Font->setSizePolicy( QSizePolicy( QSizePolicy::Minimum,
                                           QSizePolicy::Fixed ) );
    layout->addWidget( kcfg_Font, 3, 1, 1, 2 );

    QLabel *label_TitleFont = new QLabel( i18n( "Title font:" ), this );
    layout->addWidget( label_TitleFont, 2, 0 );

    KFontRequester *kcfg_TitleFont = new KFontRequester( this );
    kcfg_TitleFont->setObjectName( QLatin1String("kcfg_TitleFont") );
    kcfg_TitleFont->setSizePolicy( QSizePolicy( QSizePolicy::Minimum,
                                                QSizePolicy::Fixed ) );
    layout->addWidget( kcfg_TitleFont, 2, 1, 1, 2 );
    layout->setRowStretch(4,1);
}

KNoteEditorConfig::KNoteEditorConfig( const KComponentData &inst, QWidget *parent )
    :KCModule( inst, parent )
{
    QVBoxLayout *lay = new QVBoxLayout( this );
    QWidget * w =  new KNoteEditorConfigWidget( true );
    lay->addWidget( w );
    lay->addStretch();
    addConfig( KNotesGlobalConfig::self(), w );
    load();
}

void KNoteEditorConfig::save()
{
    KCModule::save();
}

void KNoteEditorConfig::load()
{
    KCModule::load();
}

KNoteActionConfig::KNoteActionConfig(const KComponentData &inst, QWidget *parent )
    :KCModule( inst, parent )
{
    QVBoxLayout *lay = new QVBoxLayout( this );
    QWidget * w =  new QWidget( this );
    lay->addWidget( w );
    QGridLayout *layout = new QGridLayout( w );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setMargin( 0 );

    QLabel *label_MailAction = new QLabel( i18n( "&Mail action:" ), this );
    layout->addWidget( label_MailAction, 0, 0 );

    KLineEdit *kcfg_MailAction = new KLineEdit( this );
    kcfg_MailAction->setObjectName( QLatin1String("kcfg_MailAction") );
    label_MailAction->setBuddy( kcfg_MailAction );
    layout->addWidget( kcfg_MailAction, 0, 1 );
    addConfig( KNotesGlobalConfig::self(), w );
    lay->addStretch();
    load();
}

void KNoteActionConfig::save()
{
    KCModule::save();
}

void KNoteActionConfig::load()
{
    KCModule::load();
}

KNoteNetworkConfig::KNoteNetworkConfig(const KComponentData &inst, QWidget *parent )
    :KCModule( inst, parent )
{
    QVBoxLayout *lay = new QVBoxLayout( this );
    QWidget * w =  new QWidget( this );
    lay->addWidget( w );
    QVBoxLayout *layout = new QVBoxLayout( w );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setMargin( 0 );

    QGroupBox *incoming = new QGroupBox( i18n( "Incoming Notes" ) );
    QHBoxLayout *tmpLayout = new QHBoxLayout;

    QCheckBox *tmpChkB=new QCheckBox( i18n( "Accept incoming notes" ) );
    tmpChkB->setObjectName( QLatin1String("kcfg_ReceiveNotes") );
    tmpLayout->addWidget( tmpChkB );
    incoming->setLayout( tmpLayout );
    layout->addWidget( incoming );

    QGroupBox *outgoing = new QGroupBox( i18n( "Outgoing Notes" ) );
    tmpLayout = new QHBoxLayout;

    QLabel *label_SenderID = new QLabel( i18n( "&Sender ID:" ) );
    KLineEdit *kcfg_SenderID = new KLineEdit;
    kcfg_SenderID->setClearButtonShown(true);
    kcfg_SenderID->setObjectName( QLatin1String("kcfg_SenderID") );
    label_SenderID->setBuddy( kcfg_SenderID );
    tmpLayout->addWidget( label_SenderID );
    tmpLayout->addWidget( kcfg_SenderID );
    outgoing->setLayout( tmpLayout );
    layout->addWidget( outgoing );

    tmpLayout = new QHBoxLayout;

    QLabel *label_Port = new QLabel( i18n( "&Port:" ) );

    tmpLayout->addWidget( label_Port );

    KIntNumInput *kcfg_Port = new KIntNumInput;
    kcfg_Port->setObjectName( QLatin1String("kcfg_Port") );
    kcfg_Port->setRange( 0, 65535 );
    kcfg_Port->setSliderEnabled( false );
    label_Port->setBuddy( kcfg_Port );
    tmpLayout->addWidget( kcfg_Port );
    layout->addLayout( tmpLayout );
    lay->addStretch();
    addConfig( KNotesGlobalConfig::self(), w );
    load();
}

void KNoteNetworkConfig::save()
{
    KCModule::save();
}

void KNoteNetworkConfig::load()
{
    KCModule::load();
}

#include "knoteconfigdlg.moc"
