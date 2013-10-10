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

#ifndef KNOTESLEGACY_H
#define KNOTESLEGACY_H

#include <QDir>
#include <QString>

namespace KCal {
class CalendarLocal;
class Journal;
}


/**
 * This converts and removes old note storage and config files.
 */
class KNotesLegacy
{
  public:
    static void cleanUp();
    static bool convert( KCal::CalendarLocal *calendar );
    
  private:
    static bool convertKNotes1Config( KCal::Journal *j, QDir &dir,
                                      const QString &file );
    static bool convertKNotes2Config( KCal::Journal *j, QDir &dir,
                                      const QString &file );
};

#endif
