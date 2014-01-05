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

#include "knoteskeydialog.h"

#include <KShortcutsEditor>
#include <KDialog>
#include <KLocalizedString>
#include <KSharedConfig>

KNotesKeyDialog::KNotesKeyDialog( KActionCollection *globals, QWidget *parent )
    : KDialog( parent )
{
    setCaption( i18n( "Configure Shortcuts" ) );
    setButtons( Default | Ok | Cancel );

    m_keyChooser = new KShortcutsEditor( globals, this );
    setMainWidget( m_keyChooser );
    connect( this, SIGNAL(defaultClicked()),
             m_keyChooser, SLOT(allDefault()) );
    readConfig();
}

KNotesKeyDialog::~KNotesKeyDialog()
{
    writeConfig();
}

void KNotesKeyDialog::readConfig()
{
    KConfigGroup grp( KGlobal::config(), "KNotesKeyDialog" );
    const QSize size = grp.readEntry( "Size", QSize(300, 200) );
    if ( size.isValid() ) {
        resize( size );
    }
}

void KNotesKeyDialog::writeConfig()
{
    KConfigGroup grp( KGlobal::config(), "KNotesKeyDialog" );
    grp.writeEntry( "Size", size() );
    grp.sync();
}

void KNotesKeyDialog::insert( KActionCollection *actions )
{
    m_keyChooser->addCollection( actions, i18n( "Note Actions" ) );
}

void KNotesKeyDialog::save()
{
    m_keyChooser->save();
}
