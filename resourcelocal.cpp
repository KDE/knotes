/*
  This file is part of KNotes.

  Copyright (c) 2004 Bo Thorsen <bo@klaralvdalens-datakonsult.se>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston,
  MA  02111-1307, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt.  If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.
*/

#include "resourcelocal.h"
#include "knoteslegacy.h"
#include "resourcemanager.h"

#include <libkcal/icalformat.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kio/netaccess.h>

#include <qdir.h>


ResourceLocal::ResourceLocal( const KConfig* config )
    : ResourceNotes( config )
{
}

ResourceLocal::~ResourceLocal()
{
}

bool ResourceLocal::load()
{
    mCalendar.load( KGlobal::dirs()->saveLocation( "appdata" )
                     + "notes.ics" );

    // TODO
    // initialize the Calendar
    //mCalendar.setOwner(..);
    //mCalendar.setEmail(..);

    // read the old config files into mCalendar and convert them
    if ( KNotesLegacy::convert( &mCalendar ) )
        manager()->save();

    KCal::Journal::List notes = mCalendar.journals();
    KCal::Journal::List::ConstIterator it;
    for ( it = notes.begin(); it != notes.end(); ++it )
        manager()->registerNote( this, *it, true );

    return true;
}

bool ResourceLocal::save()
{
    QString file = KGlobal::dirs()->saveLocation( "appdata" ) + "notes.ics";
    QString backup = file + "~";

    // if the backup fails don't even try to save the current notes
    // (might just destroy the file that's already there)

    if ( KIO::NetAccess::exists( KURL( file ), true, 0 ) &&
         !KIO::NetAccess::file_copy( KURL( file ), KURL( backup ), -1, true ) )
    {
        KMessageBox::error( 0,
                            i18n("<qt>Unable to save the notes backup to "
                                 "<b>%1</b>. Check that there is sufficient "
                                 "disk space.</qt>").arg( backup ) );
        return false;
    } 
    else if ( !mCalendar.save( file, new KCal::ICalFormat() ) ) 
    {
        KMessageBox::error( 0,
                            i18n("<qt>Unable to save the notes to <b>%1</b>. "
                                 "Check that there is sufficient disk space."
                                 "<br>There should be a backup in <b>%2</b> "
                                 "though.</qt>").arg( file ).arg( backup ) );
        return false;
    }

    return true;
}

bool ResourceLocal::addNote( KCal::Journal* journal )
{
    mCalendar.addJournal( journal );
    manager()->registerNote( this, journal, false );
    return true;
}

bool ResourceLocal::deleteNote( KCal::Journal* journal )
{
    mCalendar.deleteJournal( journal );
    return true;
}
