/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 2002-2004, Michael Brade <brade@kde.org>

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

#include "knoteslegacy.h"
#include "knoteconfig.h"
#include "kdepim-version.h"

#include <QFile>
#include <QFont>
#include <QPoint>
#include <QColor>
#include <QStringList>
#include <QTextStream>

#include <kdebug.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kurl.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kio/netaccess.h>
#include <kcal/calendarlocal.h>
#include <kcal/journal.h>

#include <unistd.h>
#include <netwm.h>

using namespace KCal;


void KNotesLegacy::cleanUp()
{
    // remove old (KDE 1.x) local config file if it still exists
    const QString configfile = KGlobal::dirs()->saveLocation( "config" ) + QLatin1String("knotesrc");
    if ( QFile::exists( configfile ) ) {
        KConfigGroup test(
                    KSharedConfig::openConfig( configfile, KConfig::SimpleConfig ),
                    "General" );
        double version = test.readEntry( "version", 9999999.99 );

        if ( version == 1.0 ) {
            if ( !( KStandardDirs::checkAccess( configfile, W_OK ) &&
                    QFile::remove( configfile ) ) ) {
                kError( 5500 ) << "Could not delete old config file" << configfile;
            }
        }
    }
}

bool KNotesLegacy::convert( CalendarLocal *calendar )
{
    bool converted = false;

    QDir noteDir( KGlobal::dirs()->saveLocation( "appdata", QLatin1String("notes/") ) );
    const QStringList notes = noteDir.entryList( QDir::Files, QDir::Name );
    for ( QStringList::ConstIterator note = notes.constBegin(); note != notes.constEnd();
          ++note ) {
        QString file = noteDir.absoluteFilePath( *note );
        KConfig *test = new KConfig( file, KConfig::SimpleConfig );
        KConfigGroup grp( test, "General" );
        double version = grp.readEntry( "version", 1.0 );

        if ( version < 3.0 ) {

            // create the new note
            Journal *journal = new Journal();
            bool success;

            if ( version < 2.0 ) {
                success = convertKNotes1Config( journal, noteDir, *note );
            } else {
                success = convertKNotes2Config( journal, noteDir, *note );
            }

            // could not convert file => do not add a new note
            if ( !success ) {
                delete journal;
            } else {
                calendar->addJournal( journal );
                converted = true;
            }
        } else if ( version < 3.2 ) { // window state changed for version 3.2
#ifdef Q_WS_X11
            uint state = grp.readEntry( "state", uint( NET::SkipTaskbar ) );

            grp.writeEntry( "ShowInTaskbar",
                            ( state & NET::SkipTaskbar ) ? false : true );
            grp.writeEntry( "KeepAbove",
                            ( state & NET::KeepAbove ) ? true : false );
#endif
            grp.deleteEntry( "state" );
        }
        delete test;
    }

    return converted;
}

bool KNotesLegacy::convertKNotes1Config( Journal *journal, QDir &noteDir,
                                         const QString &file )
{
    QFile infile( noteDir.absoluteFilePath( file ) );
    if ( !infile.open( QIODevice::ReadOnly ) ) {
        kError( 5500 ) << "Could not open input file: \""
                       << infile.fileName() << "\"";
        return false;
    }

    QTextStream input( &infile );

    // get the name
    journal->setSummary( input.readLine() );

    const QStringList props = input.readLine().split( QLatin1Char('+'), QString::SkipEmptyParts );

    // robustness
    if ( props.count() != 13 ) {
        kWarning( 5500 ) <<"The file \"" << infile.fileName()
                        << "\" lacks version information but is not a valid"
                        << "KNotes 1 config file either!";
        return false;
    }

    // the new configfile's name
    const QString configFile = noteDir.absoluteFilePath( journal->uid() );

    // set the defaults
    KIO::NetAccess::file_copy(
                KUrl( KGlobal::dirs()->saveLocation( "config" ) + QLatin1String("knotesrc") ),
                KUrl( configFile ),
                0
                );

    KNoteConfig config( KSharedConfig::openConfig( configFile,
                                                   KConfig::NoGlobals ) );
    config.readConfig();
    config.setVersion( QLatin1String(KDEPIM_VERSION) );

    // get the geometry
    config.setWidth( props[3].toUInt() );
    config.setHeight( props[4].toUInt() );

    // get the background color
    uint red = input.readLine().toUInt();
    uint green = input.readLine().toUInt();
    uint blue = input.readLine().toUInt();
    config.setBgColor( QColor( red, green, blue ) );

    // get the foreground color
    red = input.readLine().toUInt();
    green = input.readLine().toUInt();
    blue = input.readLine().toUInt();
    config.setFgColor( QColor( red, green, blue ) );

    // get the font
    QString fontfamily = input.readLine();
    if ( fontfamily.isEmpty() )
        fontfamily = QLatin1String( "Sans Serif" );
    uint size = input.readLine().toUInt();
    size = qMax( size, ( uint ) 4 );
    uint weight = input.readLine().toUInt();
    bool italic = ( input.readLine().toUInt() == 1 );
    QFont font( fontfamily, size, weight, italic );

    config.setTitleFont( font );
    config.setFont( font );

    // 3d frame? Not supported yet!
    input.readLine();

    // autoindent
    config.setAutoIndent( input.readLine().toUInt() == 1 );

    // KNotes 1 never had rich text
    config.setRichText( false );

    int note_desktop = props[0].toUInt();

    // hidden or on all desktops?
    if ( input.readLine().toUInt() == 1 )
        note_desktop = 0;
#ifdef Q_WS_X11
    else if ( props[11].toUInt() == 1 )
        note_desktop = NETWinInfo::OnAllDesktops;
#endif

    config.setDesktop( note_desktop );
    config.setPosition( QPoint( props[1].toUInt(), props[2].toUInt() ) );
    config.setKeepAbove( props[12].toUInt() & 2048 );

    config.writeConfig();

    // get the text
    QString text;
    while ( !input.atEnd() ) {
        text.append( input.readLine() );
        if ( !input.atEnd() ) {
            text.append( QLatin1Char('\n') );
        }
    }

    journal->setDescription( text );

    if ( !infile.remove() )
    {
        kWarning( 5500 ) << "Could not delete input file: \"" << infile.fileName()
                         << "\"";
    }

    return true;
}

bool KNotesLegacy::convertKNotes2Config( Journal *journal, QDir &noteDir,
                                         const QString &file )
{
    const QString configFile = noteDir.absoluteFilePath( journal->uid() );

    // new name for config file
    if ( !noteDir.rename( file, journal->uid() ) ) {
        kError( 5500 ) << "Could not rename input file: \""
                       << noteDir.absoluteFilePath( file ) << "\" to \""
                       << configFile << "\"!";
        return false;
    }

    // update the config
    KConfig config( configFile );
    KConfigGroup grp( &config, "Data" );
    journal->setSummary( grp.readEntry( "name" ) );
    config.deleteGroup( "Data", KConfig::Localized );
    KConfigGroup _grp2(&config, "General" ); // XXX right?
    _grp2.writeEntry( "version", KDEPIM_VERSION );
    KConfigGroup _grp3(&config, "WindowDisplay" ); // XXX right?
#ifdef Q_WS_X11
    uint state = _grp3.readEntry( "state", uint( NET::SkipTaskbar ) );
    _grp3.writeEntry( "ShowInTaskbar",
                      ( state & NET::SkipTaskbar ) ? false : true );
    _grp3.writeEntry( "KeepAbove",
                      ( state & NET::KeepAbove ) ? true : false );
#endif
    _grp3.deleteEntry( "state" );

    // load the saved text and put it in the journal
    QFile infile( noteDir.absoluteFilePath( QLatin1Char('.') + file + QLatin1String("_data") ) );
    if ( infile.open( QIODevice::ReadOnly ) ) {
        QTextStream input( &infile );
        input.setCodec( "UTF-8" );
        journal->setDescription( input.readAll() );
        if ( !infile.remove() ) {
            kWarning( 5500 ) << "Could not delete data file: \"" << infile.fileName()
                             << "\"";
        }
    }
    else
        kWarning( 5500 ) << "Could not open data file: \"" << infile.fileName()
                         << "\"";

    return true;
}
