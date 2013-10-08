/*******************************************************************
 This file is part of KNotes.

 Copyright (c) 2004, Bo Thorsen <bo@sonofthor.dk>

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA  02110-1301, USA.

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
*******************************************************************/

#ifndef RESOURCELOCAL_H
#define RESOURCELOCAL_H

#include <kurl.h>

#include <kcal/calendarlocal.h>

#include "resourcenotes.h"


class ResourceLocal
  : public ResourceNotes
{
  public:
    ResourceLocal();
    explicit ResourceLocal( const KConfigGroup &group );
    virtual ~ResourceLocal();
    
    virtual void writeConfig( KConfigGroup &group );
    
    /**
     * Load resource data.
     */
    virtual bool load();
    
    /**
     * Save resource data.
     */
    virtual bool save();
    
    virtual KUrl url() const { return mURL; }
    virtual void setURL( const KUrl &url ) { mURL = url; }
    
    virtual bool addNote( KCal::Journal * );
    virtual bool deleteNote( KCal::Journal * );
    
    virtual KCal::Alarm::List alarms( const KDateTime &from,
                                      const KDateTime &to );
    
  protected:
    KCal::CalendarLocal mCalendar;
    KUrl mURL;
};


#endif // RESOURCELOCAL_H
