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

#include <qfile.h>
#include <qfont.h>
#include <qpoint.h>
#include <qcolor.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kurl.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kio/netaccess.h>

#include <unistd.h>

#include "knoteslegacy.h"
#include "knoteconfig.h"
#include "version.h"

#include "libkcal/calendarlocal.h"
#include "libkcal/journal.h"

#include <netwm.h>

using namespace KCal;


void KNotesLegacy::cleanUp()
{
    // remove old (KDE 1.x) local config file if it still exists
    QString configfile = KGlobal::dirs()->saveLocation( "config" ) + "knotesrc";
    if ( QFile::exists( configfile ) ) {
        KSimpleConfig *test = new KSimpleConfig( configfile );
        test->setGroup( "General" );
        double version = test->readDoubleNumEntry( "version", 1.0 );
        delete test;

        if ( version == 1.0 ) {
            if ( !( checkAccess( configfile, W_OK ) &&
                    QFile::remove( configfile ) ) )
            {
                kdError(5500) << k_funcinfo << "Could not delete old config file " << configfile << endl;
            }
        }
    }
}

bool KNotesLegacy::convert( CalendarLocal *calendar )
{
    bool converted = false;

    QDir noteDir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
    QStringList notes = noteDir.entryList( QDir::Files, QDir::Name );
    for ( QStringList::Iterator note = notes.begin(); note != notes.end(); note++ )
    {
        QString file = noteDir.absFilePath( *note );
        KSimpleConfig* test = new KSimpleConfig( file );
        test->setGroup( "General" );
        double version = test->readDoubleNumEntry( "version", 1.0 );

        if ( version < 3.0 )
        {

            // create the new note
            Journal *journal = new Journal();
            bool success;

            if ( version < 2.0 )
                success = convertKNotes1Config( journal, noteDir, *note );
            else
                success = convertKNotes2Config( journal, noteDir, *note );

            // could not convert file => do not add a new note
            if ( !success )
                delete journal;
            else
            {
                calendar->addJournal( journal );
                converted = true;
            }
        }
        // window state changed for version 3.2
        else if ( version < 3.2 )
        {
            uint state = test->readUnsignedLongNumEntry( "state", NET::SkipTaskbar );
            test->writeEntry( "ShowInTaskbar", (state & NET::SkipTaskbar) ? false : true );
            test->writeEntry( "KeepAbove", (state & NET::KeepAbove) ? true : false );
            test->deleteEntry( "state" );
        }
        delete test;
    }

    return converted;
}

bool KNotesLegacy::convertKNotes1Config( Journal *journal, QDir& noteDir,
        const QString& file )
{
    QFile infile( noteDir.absFilePath( file ) );
    if ( !infile.open( IO_ReadOnly ) )
    {
        kdError(5500) << k_funcinfo << "Could not open input file: \""
                      << infile.name() << "\"" << endl;
        return false;
    }

    QTextStream input( &infile );

    // get the name
    journal->setSummary( input.readLine() );

    QStringList props = QStringList::split( '+', input.readLine() );

    // robustness
    if ( props.count() != 13 )
    {
        kdWarning(5500) << k_funcinfo << "The file \"" << infile.name()
                        << "\" lacks version information but is not a valid "
                        << "KNotes 1 config file either!" << endl;
        return false;
    }

    // the new configfile's name
    QString configFile = noteDir.absFilePath( journal->uid() );

    // set the defaults
    KIO::NetAccess::copy(
        KURL( KGlobal::dirs()->saveLocation( "config" ) + "knotesrc" ),
        KURL( configFile ),
        0
    );

    KNoteConfig config( KSharedConfig::openConfig( configFile, false, false ) );
    config.readConfig();
    config.setVersion( KNOTES_VERSION );

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
        fontfamily = QString( "Sans Serif" );
    uint size = input.readLine().toUInt();
    size = QMAX( size, 4 );
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
    else if ( props[11].toUInt() == 1 )
        note_desktop = NETWinInfo::OnAllDesktops;

    config.setDesktop( note_desktop );
    config.setPosition( QPoint( props[1].toUInt(), props[2].toUInt() ) );
    config.setKeepAbove( props[12].toUInt() & 2048 );

    config.writeConfig();

    // get the text
    QString text;
    while ( !input.atEnd() )
    {
        text.append( input.readLine() );
        if ( !input.atEnd() )
            text.append( '\n' );
    }

    journal->setDescription( text );

    if ( !infile.remove() )
        kdWarning(5500) << k_funcinfo << "Could not delete input file: \"" << infile.name() << "\"" << endl;

    return true;
}

bool KNotesLegacy::convertKNotes2Config( Journal *journal, QDir& noteDir,
        const QString& file )
{
    QString configFile = noteDir.absFilePath( journal->uid() );

    // new name for config file
    if ( !noteDir.rename( file, journal->uid() ) )
    {
        kdError(5500) << k_funcinfo << "Could not rename input file: \""
                      << noteDir.absFilePath( file ) << "\" to \""
                      << configFile << "\"!" << endl;
        return false;
    }

    // update the config
    KConfig config( configFile );
    config.setGroup( "Data" );
    journal->setSummary( config.readEntry( "name" ) );
    config.deleteGroup( "Data", true );
    config.setGroup( "General" );
    config.writeEntry( "version", KNOTES_VERSION );
    config.setGroup( "WindowDisplay" );
    uint state = config.readUnsignedLongNumEntry( "state", NET::SkipTaskbar );
    config.writeEntry( "ShowInTaskbar", (state & NET::SkipTaskbar) ? false : true );
    config.writeEntry( "KeepAbove", (state & NET::KeepAbove) ? true : false );
    config.deleteEntry( "state" );

    // load the saved text and put it in the journal
    QFile infile( noteDir.absFilePath( "." + file + "_data" ) );
    if ( infile.open( IO_ReadOnly ) )
    {
        QTextStream input( &infile );
        input.setEncoding( QTextStream::UnicodeUTF8 );
        journal->setDescription( input.read() );
        if ( !infile.remove() )
            kdWarning(5500) << k_funcinfo << "Could not delete data file: \"" << infile.name() << "\"" << endl;
    }
    else
        kdWarning(5500) << k_funcinfo << "Could not open data file: \"" << infile.name() << "\"" << endl;

    return true;
}
