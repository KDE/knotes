/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 2002, Michael Brade <brade@kde.org>

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
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
#include <kstandarddirs.h>
#include <kurl.h>
#include <ksimpleconfig.h>
#include <kio/netaccess.h>

#include <unistd.h>

#include "version.h"
#include "knoteslegacy.h"

#include "libkcal/calendarlocal.h"
#include "libkcal/journal.h"

#include <netwm.h>

using namespace KCal;


void KNotesLegacy::cleanUp()
{
    // remove old (KDE 1.x) local config file if it still exists
    QString configfile = KGlobal::dirs()->findResource( "config", "knotesrc" );
    KSimpleConfig *test = new KSimpleConfig( configfile );
    test->setGroup( "General" );
    double version = test->readDoubleNumEntry( "version", 1 );
    if ( version == 1 )
    {
        delete test;
        if ( !( checkAccess( configfile, W_OK ) &&
                KIO::NetAccess::del( KURL(configfile) ) ) )
        {
            kdError(5500) << k_funcinfo << "Could not delete old config file!!" << endl;
            // TODO
        }
    }
    else if ( version < 3 )
    {
        test->writeEntry( "version", KNOTES_VERSION );
        delete test;
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
        KSimpleConfig* test = new KSimpleConfig( file, true );
        test->setGroup( "General" );
        double version = test->readDoubleNumEntry( "version", 1 );
        delete test;
        
        if ( version < 3.0 )
        {
            // create the new note
            Journal *journal = new Journal();

            if ( version < 2.0 )
                convertKNotes1Config( journal, noteDir, *note );
            else
                convertKNotes2Config( journal, noteDir, *note );
            
            calendar->addJournal( journal );
            converted = true;
        }
    }
    
    return converted;
}

void KNotesLegacy::convertKNotes1Config( Journal *journal, QDir& noteDir, 
        const QString& file )
{
    QFile infile( noteDir.absFilePath( file ) );

    if ( !infile.open( IO_ReadOnly ) )
    {
        kdError(5500) << k_funcinfo << "Could not open input file: " << infile.name() << endl;
        
        // TODO: better return false and delete current journal, same in convertKNotes2Config
        return;
    }

    QTextStream input( &infile );

    // set the new configfile's name...
    QString configFile = noteDir.absFilePath( journal->uid() );

    // set the defaults
    KIO::NetAccess::copy(
        KURL( KGlobal::dirs()->findResource( "config", "knotesrc" ) ), KURL( configFile )
    );

    // get the name
    journal->setSummary( input.readLine() );

    // TODO: Needed? What about KConfig? This deletes everything else?
    //       Test with a config file that contains a value not set here!
    KSimpleConfig config( configFile );
    
    config.setGroup( "General" );
    config.writeEntry( "version", KNOTES_VERSION );
    
    // use the new default for this group
    config.setGroup( "Actions" );
    config.writeEntry( "mail", "kmail --msg %f" );

    config.setGroup( "Display" );

    // get the geometry
    QString geo = input.readLine();

    int pos, data[13];
    int n = 0;

    while ( (pos = geo.find('+')) != -1 )
    {
        if( n < 13 )
            data[n++] = geo.left(pos).toInt();
        geo.remove( 0, pos + 1 );
    }
    if ( n < 13 )
        data[n++] = geo.toInt();

    config.writeEntry( "width", data[3] );
    config.writeEntry( "height", data[4] );

    // get the background color
    uint red = input.readLine().toUInt();
    uint green = input.readLine().toUInt();
    uint blue = input.readLine().toUInt();
    config.writeEntry( "bgcolor", QColor( red, green, blue ) );

    // get the foreground color
    red = input.readLine().toUInt();
    green = input.readLine().toUInt();
    blue = input.readLine().toUInt();
    config.writeEntry( "fgcolor", QColor( red, green, blue ) );

    config.setGroup( "Editor" );

    // get the font
    QString fontfamily = input.readLine();
    if ( fontfamily.isEmpty() )
        fontfamily = QString( "helvetica" );
    uint size = input.readLine().toUInt();
    size = QMAX( size, 4 );
    uint weight = input.readLine().toUInt();
    bool italic = ( input.readLine().toUInt() == 1 );
    QFont font( fontfamily, size, weight, italic );
    
    config.writeEntry( "titlefont", font );
    config.writeEntry( "font", font );
    
    // 3d frame? Not supported yet!
    input.readLine();

    // autoindent
    bool indent = ( input.readLine().toUInt() == 1 );
    config.writeEntry( "autoindent", indent );

    // rich text and tabsize
    config.writeEntry( "richtext", false );
    config.writeEntry( "tabsize", 4 );

    config.setGroup( "WindowDisplay" );
    
    // hidden
    bool hidden = ( input.readLine().toUInt() == 1 );
    
    int note_desktop = data[0];
    if ( hidden )
        note_desktop = 0;
    else if ( data[11] == 1 )
        note_desktop = NETWinInfo::OnAllDesktops;

    config.writeEntry( "desktop", note_desktop );

    if ( data[1] >= 0 && data[2] >= 0 )   // just to be sure...
        config.writeEntry( "position", QPoint( data[1], data[2] ) );
    else
        config.writeEntry( "position", QPoint( 10, 10 ) );

    if ( data[12] & 2048 )
        config.writeEntry( "state", NET::SkipTaskbar | NET::StaysOnTop );
    else
        config.writeEntry( "state", NET::SkipTaskbar );

    config.sync();

    // get the text
    QString text;    
    while ( !input.atEnd() ) 
    {
        text.append( input.readLine() );
        if ( !input.atEnd() )
            text.append( '\n' );
    }

    journal->setDescription( text );
    journal->addAttachment( new Attachment( configFile, CONFIG_MIME ) );
    
    infile.close();
    infile.remove();        // TODO: success?
}

void KNotesLegacy::convertKNotes2Config( Journal *journal, QDir& noteDir, 
        const QString& file )
{
    // new name for config file
    noteDir.rename( file, journal->uid() );
    QString configFile = noteDir.absFilePath( journal->uid() );
    
    // update the config
    KConfig config( configFile );
    config.setGroup( "Data" );
    journal->setSummary( config.readEntry( "name", QString::null ) );
    config.deleteEntry( "name" );
    config.deleteGroup( "Data", false );
    config.setGroup( "General" );
    config.writeEntry( "version", KNOTES_VERSION );

    // load the saved text and put it in the journal
    QFile infile( noteDir.absFilePath( "." + file + "_data" ) );
    if ( infile.open( IO_ReadOnly ) )
    {
        QTextStream input( &infile );
        input.setEncoding( QTextStream::UnicodeUTF8 );
        journal->setDescription( input.read() );
        infile.close();
        infile.remove();    // TODO: success?
    } 
    else
        kdError(5500) << k_funcinfo << "Could not open input file: " << infile.name() << endl;
 
    journal->addAttachment( new Attachment( configFile, CONFIG_MIME ) );
}
