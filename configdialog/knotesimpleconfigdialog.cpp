/*
  Copyright (c) 2013, 2014 Montel Laurent <montel@kde.org>

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
#include "knoteeditorconfigwidget.h"

#include "noteshared/attributes/notedisplayattribute.h"

#include <KLocalizedString>
#include <KWindowSystem>
#include <KGlobal>

#include <QTabWidget>
#include <QApplication>

KNoteSimpleConfigDialog::KNoteSimpleConfigDialog( const QString &title,
                                                  QWidget *parent )
    : KDialog( parent )
{
    setButtons( /*Default |*/ Ok | Cancel  );
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
    mTabWidget = new QTabWidget;

    mEditorConfigWidget = new KNoteEditorConfigWidget(true, this);
    mTabWidget->addTab(mEditorConfigWidget, i18n( "Editor Settings" ));

    mDisplayConfigWidget = new KNoteDisplayConfigWidget(true, this);
    mTabWidget->addTab(mDisplayConfigWidget, i18n( "Display Settings" ));

    setMainWidget(mTabWidget);
    readConfig();
}

KNoteSimpleConfigDialog::~KNoteSimpleConfigDialog()
{
    writeConfig();
}

void KNoteSimpleConfigDialog::load(Akonadi::Item &item, bool isRichText)
{
    NoteShared::NoteDisplayAttribute *attr = item.attribute<NoteShared::NoteDisplayAttribute>();
    mEditorConfigWidget->load(attr, isRichText);
    mDisplayConfigWidget->load(attr);
}

void KNoteSimpleConfigDialog::slotUpdateCaption(const QString & name)
{
    setCaption( name );
}

void KNoteSimpleConfigDialog::save(Akonadi::Item &item, bool &isRichText)
{
    NoteShared::NoteDisplayAttribute *attr =  item.attribute<NoteShared::NoteDisplayAttribute>( Akonadi::Entity::AddIfMissing );
    mEditorConfigWidget->save(attr, isRichText);
    mDisplayConfigWidget->save(attr);
}

void KNoteSimpleConfigDialog::readConfig()
{
    KConfigGroup group( KGlobal::config(), "KNoteSimpleConfigDialog" );
    const QSize size = group.readEntry( "Size", QSize(600, 400) );
    if ( size.isValid() ) {
        resize( size );
    }
}

void KNoteSimpleConfigDialog::writeConfig()
{
    KConfigGroup group( KGlobal::config(), "KNoteSimpleConfigDialog" );
    group.writeEntry( "Size", size() );
    group.sync();
}
