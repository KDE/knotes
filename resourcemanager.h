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


#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <resourcenotes.h>

#include <kresources/manager.h>

class KNotesApp;
class KNote;

namespace KCal {
    class Journal;
}


class ResourceManager : public QObject,
                        public KRES::ManagerObserver<ResourceNotes>
{
    Q_OBJECT

public:
    ResourceManager( KNotesApp* app );
    virtual ~ResourceManager();

    void load();
    void save();
    void saveConfigs();

    QString newNote( const QString& name, const QString& text );

    int count();

    QMap<QString,QString> notes() const;

    void sync( const QString& app );

    KNote* first();

    void showNextNote();

    KNote* note( const QString& );

    QDictIterator<KNote> iterator()
        { return QDictIterator<KNote>( m_noteList ); }

    void resourceAdded( ResourceNotes* resource );
    void resourceModified( ResourceNotes* resource );
    void resourceDeleted( ResourceNotes* resource );

    void registerNote( ResourceNotes* resource,
                       KCal::Journal* journal, bool loaded );

private slots:
    void slotNoteKilled( KCal::Journal* );

private:
    KNotesApp* m_app;
    KRES::Manager<ResourceNotes>* m_manager;
    QMap<KNote*, ResourceNotes*> m_resourceMap;
    QDict<KNote> m_noteList;
};


#endif // RESOURCEMANAGER_H
