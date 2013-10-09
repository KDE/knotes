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

#include "knotes/resource/resourcemanager.h"
#include "knotes/resource/resourcelocal.h"

#include <kcal/journal.h>


KNotesResourceManager::KNotesResourceManager()
  : QObject( 0 )
{
  setObjectName( QLatin1String("KNotes Resource Manager") );
  m_manager = new KRES::Manager<ResourceNotes>( QLatin1String("notes") );
  m_manager->addObserver( this );
  m_manager->readConfig();
}

KNotesResourceManager::~KNotesResourceManager()
{
  delete m_manager;
}

void KNotesResourceManager::load()
{
  if ( !m_manager->standardResource() ) {
    kWarning( 5500 ) << "No standard resource yet.";
    ResourceNotes *resource = new ResourceLocal();
    m_manager->add( resource );
    m_manager->setStandardResource( resource );
  }

  // Open all active resources
  KRES::Manager<ResourceNotes>::ActiveIterator it;
  KRES::Manager<ResourceNotes>::ActiveIterator end( m_manager->activeEnd() );
  for ( it = m_manager->activeBegin(); it != end; ++it ) {
    if ( (*it)->isOpen() ) {
      kDebug(5500) << (*it)->resourceName() << " is already open";
      continue;
    }

    kDebug( 5500 ) << QLatin1String("Opening resource ") + (*it)->resourceName();
    (*it)->setManager( this );
    if ( (*it)->open() ) {
      ( *it )->load();
    }
  }
}

void KNotesResourceManager::save()
{
  KRES::Manager<ResourceNotes>::ActiveIterator it;
  KRES::Manager<ResourceNotes>::ActiveIterator end(m_manager->activeEnd());
  for ( it = m_manager->activeBegin(); it != end; ++it ) {
    ( *it )->save();
  }
}

// when adding a new note, make sure a config file exists!!

bool KNotesResourceManager::addNewNote( KCal::Journal *journal )
{
  // TODO: Make this configurable
  ResourceNotes *resource = m_manager->standardResource();
  if ( resource ) {
    if ( resource->addNote( journal ) ) {
      registerNote( resource, journal );
      return true;
    }
  } else {
    kWarning( 5500 ) << "no resource!";
  }
  return false;
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
  if ( !journal ) {
      return;
  }

  const QString uid = journal->uid();

  // Remove the journal from the resource it came from
  ResourceNotes *res = m_resourceMap.value( uid );
  if ( res ) {
    res->deleteNote( journal );
    m_resourceMap.remove( uid );

    // libkcal does not delete the journal immediately, therefore it is ok to
    // emit the journal here
    emit sigDeregisteredNote( journal );
  }
}

KCal::Alarm::List KNotesResourceManager::alarms( const KDateTime &from,
                                                 const KDateTime &to )
{
  KCal::Alarm::List result;

  KRES::Manager<ResourceNotes>::ActiveIterator it;
  for ( it = m_manager->activeBegin(); it != m_manager->activeEnd(); ++it ) {
    KCal::Alarm::List list = ( *it )->alarms( from, to );
    KCal::Alarm::List::ConstIterator itList;
    KCal::Alarm::List::ConstIterator end(list.constEnd());
    for ( itList = list.constBegin(); itList != end; ++itList ) {
      result.append( *itList );
    }
  }

  return result;
}

void KNotesResourceManager::resourceAdded( ResourceNotes *resource )
{
  kDebug( 5500 ) << "Resource added:" << resource->resourceName();

  if ( !resource->isActive() ) {
    return;
  }

  if ( resource->isOpen() ) {
    kDebug(5500) << resource->resourceName() << " is already open";
    return;
  }

  resource->setManager( this );
  if ( resource->open() ) {
    resource->load();
  }
}

void KNotesResourceManager::resourceModified( ResourceNotes *resource )
{
  kDebug( 5500 ) << "Resource modified:" << resource->resourceName();
}

void KNotesResourceManager::resourceDeleted( ResourceNotes *resource )
{
  kDebug( 5500 ) << "Resource deleted:" << resource->resourceName();
}


#include "resourcemanager.moc"
