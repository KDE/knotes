/*******************************************************************
 This file is part of KNotes.

 Copyright (c) 2004, Bo Thorsen <bo@sonofthor.dk>
               2004, Michael Brade <brade@kde.org>

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
*******************************************************************/

#include "knotes/resourcemanager.h"
#include "knotes/resourcelocal.h"

#include <libkcal/journal.h>


KNotesResourceManager::KNotesResourceManager()
    : QObject( 0, "KNotes Resource Manager" )
{
    m_manager = new KRES::Manager<ResourceNotes>( "notes" );
    m_manager->addObserver( this );
    m_manager->readConfig();
}

KNotesResourceManager::~KNotesResourceManager()
{
    delete m_manager;
}

void KNotesResourceManager::load()
{
    if ( !m_manager->standardResource() )
    {
        kdWarning(5500) << "No standard resource yet." << endl;
        ResourceNotes *resource = new ResourceLocal( 0 );
        m_manager->add( resource );
        m_manager->setStandardResource( resource );
    }

    // Open all active resources
    KRES::Manager<ResourceNotes>::ActiveIterator it;
    for ( it = m_manager->activeBegin(); it != m_manager->activeEnd(); ++it )
    {
        kdDebug(5500) << "Opening resource " + (*it)->resourceName() << endl;
        (*it)->setManager( this );
        if ( (*it)->open() )
            (*it)->load();
    }
}

void KNotesResourceManager::save()
{
    KRES::Manager<ResourceNotes>::ActiveIterator it;
    for ( it = m_manager->activeBegin(); it != m_manager->activeEnd(); ++it )
        (*it)->save();
}

// when adding a new note, make sure a config file exists!!

void KNotesResourceManager::addNewNote( KCal::Journal *journal )
{
    // TODO: Make this configurable
    ResourceNotes *resource = m_manager->standardResource();
    if ( resource )
    {
        resource->addNote( journal );
        registerNote( resource, journal );
    }
    else
        kdWarning(5500) << k_funcinfo << "no resource!" << endl;
}

void KNotesResourceManager::registerNote( ResourceNotes *resource,
    KCal::Journal *journal )
{
    // TODO: only emit the signal if the journal is new?
    m_resourceMap.insert( journal->uid(), resource );
    emit sigRegisteredNote( journal );
}

void KNotesResourceManager::deleteNote( KCal::Journal *journal )
{
    QString uid = journal->uid();

    // Remove the journal from the resource it came from
    m_resourceMap[ uid ]->deleteNote( journal );
    m_resourceMap.remove( uid );

    // libkcal does not delete the journal immediately, therefore it is ok to
    // emit the journal here
    emit sigDeregisteredNote( journal );
}

KCal::Alarm::List KNotesResourceManager::alarms( const QDateTime& from, const QDateTime& to )
{
    KCal::Alarm::List result;
    // TODO
//     KRES::Manager<ResourceNotes>::ActiveIterator it;
//     for ( it = m_manager->activeBegin(); it != m_manager->activeEnd(); ++it )
//     {
//         KCal::Alarm::List list = (*it)->alarms( from, to );
//         KCal::Alarm::List::Iterator it;
//         for ( it = list.begin(); it != list.end(); ++it )
//             result.append( *it );
//     }
    return result;
}

void KNotesResourceManager::resourceAdded( ResourceNotes *resource )
{
    kdDebug(5500) << "Resource added: " << resource->resourceName() << endl;

    if ( !resource->isActive() )
        return;

    resource->setManager( this );
    if ( resource->open() )
        resource->load();
}

void KNotesResourceManager::resourceModified( ResourceNotes *resource )
{
    kdDebug(5500) << "Resource modified: " << resource->resourceName() << endl;
}

void KNotesResourceManager::resourceDeleted( ResourceNotes *resource )
{
    kdDebug(5500) << "Resource deleted: " << resource->resourceName() << endl;
}


#include "resourcemanager.moc"
