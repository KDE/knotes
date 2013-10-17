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

#include "knoteutils.h"
#include "knoteconfig.h"
#include "kdepim-version.h"
#include "knotes/knotesglobalconfig.h"

#include <KCal/Journal>
using namespace KCal;

#include <KIO/NetAccess>
#include <KSharedConfig>
#include <KStandardDirs>

KNoteConfig *KNoteUtils::createConfig(KCal::Journal *journal, QString &configPath)
{
    // the config file location
    configPath = KGlobal::dirs()->saveLocation( "data", QLatin1String("knotes/") ) + QLatin1String("notes/") + journal->uid();

    // no config file yet? -> use the default display config if available
    // we want to write to configFile, so use "false"
    const bool newNote = !KIO::NetAccess::exists( KUrl( configPath ), KIO::NetAccess::DestinationSide, 0 );

    KNoteConfig *config = new KNoteConfig( KSharedConfig::openConfig( configPath, KConfig::NoGlobals ) );
    config->readConfig();
    config->setVersion( QLatin1String(KDEPIM_VERSION) );

    if ( newNote ) {
        // until kdelibs provides copying of KConfigSkeletons (KDE 3.4)
        KNotesGlobalConfig *globalConfig = KNotesGlobalConfig::self();
        config->setBgColor( globalConfig->bgColor() );
        config->setFgColor( globalConfig->fgColor() );
        config->setWidth( globalConfig->width() );
        config->setHeight( globalConfig->height() );

        config->setFont( globalConfig->font() );
        config->setTitleFont( globalConfig->titleFont() );
        config->setAutoIndent( globalConfig->autoIndent() );
        config->setRichText( globalConfig->richText() );
        config->setTabSize( globalConfig->tabSize() );
        config->setReadOnly( globalConfig->readOnly() );

        config->setDesktop( globalConfig->desktop() );
        config->setHideNote( globalConfig->hideNote() );
        config->setPosition( globalConfig->position() );
        config->setShowInTaskbar( globalConfig->showInTaskbar() );
        config->setRememberDesktop( globalConfig->rememberDesktop() );
        config->setKeepAbove( globalConfig->keepAbove() );
        config->setKeepBelow( globalConfig->keepBelow() );

        config->writeConfig();
    }
    return config;
}


void KNoteUtils::setProperty(KCal::Journal *journal, KNoteConfig *config)
{
    // config items in the journal have priority
    QString property = journal->customProperty( "KNotes", "FgColor" );
    if ( !property.isNull() ) {
        config->setFgColor( QColor( property ) );
    } else {
        journal->setCustomProperty( "KNotes", "FgColor",
                                      config->fgColor().name() );
    }

    property = journal->customProperty( "KNotes", "BgColor" );
    if ( !property.isNull() ) {
        config->setBgColor( QColor( property ) );
    } else {
        journal->setCustomProperty( "KNotes", "BgColor",
                                      config->bgColor().name() );
    }
    property = journal->customProperty( "KNotes", "RichText" );
    if ( !property.isNull() ) {
        config->setRichText( property == QLatin1String("true") ? true : false );
    } else {
        journal->setCustomProperty( "KNotes", "RichText",
                                      config->richText() ? QLatin1String("true") : QLatin1String("false") );
    }
}


void KNoteUtils::removeNote(KCal::Journal *journal, QWidget *parent)
{
    const QString configFile = KGlobal::dirs()->saveLocation( "data", QLatin1String("knotes/") ) + QLatin1String("notes/") + journal->uid();
    qDebug()<<" configFile"<<configFile;
    if ( !KIO::NetAccess::del( KUrl( configFile ), parent ) ) {
        qDebug() <<"Can't remove the note config:" << configFile;
    }

}
