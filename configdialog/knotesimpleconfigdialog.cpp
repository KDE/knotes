/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "knotesimpleconfigdialog.h"
#include "knoteconfigdialog.h"
#include "knotedisplayconfigwidget.h"
#include "knoteconfig.h"
#include "kdepim-version.h"

#include <KLocale>
#include <KWindowSystem>


#include <QApplication>

KNoteSimpleConfigDialog::KNoteSimpleConfigDialog( KNoteConfig *config, const QString &title,
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

KNoteSimpleConfigDialog::~KNoteSimpleConfigDialog()
{
}


void KNoteSimpleConfigDialog::slotUpdateCaption(const QString & name)
{
    setCaption( name );
}

