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

#include "resourcemanager.h"
#include "resourcelocal.h"
#include "knotesapp.h"
#include "knote.h"
#include "version.h"

#include <libkcal/journal.h>
#include <libkcal/attachment.h>

#include <klocale.h>
#include <kstandarddirs.h>
#include <kio/netaccess.h>

#include <qdir.h>


ResourceManager::ResourceManager( KNotesApp* app )
    : QObject( app, "Resource Manager" ), m_app( app )
{
    m_noteList.setAutoDelete( true );

    m_manager = new KRES::Manager<ResourceNotes>( "notes" );
    m_manager->addObserver( this );
    m_manager->readConfig();
}

ResourceManager::~ResourceManager()
{
    blockSignals( true );
    m_noteList.clear();
    blockSignals( false );
}

void ResourceManager::load()
{
    if ( !m_manager->standardResource() ) {
        kdDebug() << "Warning! No standard resource yet." << endl;
        ResourceNotes *resource = new ResourceLocal( 0 );
        m_manager->add( resource );
        m_manager->setStandardResource( resource );
    }

    // Open all active resources
    KRES::Manager<ResourceNotes>::ActiveIterator it;
    for ( it = m_manager->activeBegin(); it != m_manager->activeEnd(); ++it ) {
        kdDebug() << "Opening resource " + (*it)->resourceName() << endl;
        (*it)->setManager( this );
        if ( (*it)->open() )
            (*it)->load();
    }
}

void ResourceManager::save()
{
    KRES::Manager<ResourceNotes>::ActiveIterator it;
    for ( it = m_manager->activeBegin(); it != m_manager->activeEnd(); ++it )
        (*it)->save();
}

void ResourceManager::saveConfigs()
{
    QDictIterator<KNote> it( m_noteList );
    for ( ; it.current(); ++it )
        it.current()->saveConfig();
}

QString ResourceManager::newNote( const QString& name, const QString& text )
{
    // create the new note
    KCal::Journal *note = new KCal::Journal();

    // new notes have the current date/time as title if none was given
    if ( !name.isEmpty() )
        note->setSummary( name );
    else
        note->setSummary( KGlobal::locale()->formatDateTime( QDateTime::currentDateTime() ) );
    note->setDescription( text );

    // TODO: Make this be configurable
    ResourceNotes* resource = m_manager->standardResource();
    if ( resource )
        resource->addNote( note );
    else
        kdDebug(5800) << "ResourceManager::newNote(): no resource" << endl;

    return note->uid();
}

void ResourceManager::registerNote( ResourceNotes* resource,
                                    KCal::Journal* journal, bool loaded )
{
    // KOrganizers journals don't have attachments -> use default
    // display config
    if ( journal->attachments(CONFIG_MIME).isEmpty() ) {
        // Set the name of the config file...
        QDir noteDir( KGlobal::dirs()->saveLocation( "appdata", "notes/" ) );
        QString file = noteDir.absFilePath( journal->uid() );
        KURL src ( KGlobal::dirs()->findResource( "config", "knotesrc" ) );
        KURL dst ( file );

        // ...and "fill" it with the default config
        KIO::NetAccess::file_copy( src, dst, -1, true, m_app );
        journal->addAttachment( new KCal::Attachment( file, CONFIG_MIME ) );
    }

    if ( journal->summary().isNull() && journal->dtStart().isValid() ) {
        QString s = KGlobal::locale()->formatDateTime( journal->dtStart() );
        journal->setSummary( s );
    }

    KNote* newNote = new KNote( m_app, m_app->domDocument(), journal );
    m_noteList.insert( newNote->noteId(), newNote );
    m_resourceMap[ newNote ] = resource;

    connect( newNote, SIGNAL(sigNewNote()), m_app, SLOT(slotNewNote()) );
    connect( newNote, SIGNAL(sigKillNote( KCal::Journal* )),
             this,    SLOT(slotNoteKilled( KCal::Journal* )) );
    connect( newNote, SIGNAL(sigNameChanged()),
             m_app, SLOT(updateNoteActions()) );
    connect( newNote, SIGNAL(sigSaveData()), m_app, SLOT(saveNotes()) );

    if( !loaded ) {
        // This is a new note
        m_app->updateNoteActions();
        m_app->showNote( newNote );
    }
}

void ResourceManager::slotNoteKilled( KCal::Journal* journal )
{
    // Remove the journal from the resource it came from
    m_resourceMap[ m_noteList[ journal->uid() ] ]->deleteNote( journal );
    m_resourceMap.remove( m_noteList[ journal->uid() ] );

    // Kill the KNote object
    m_noteList.remove( journal->uid() );

    QString configFile = journal->attachments( CONFIG_MIME ).first()->uri();
    if ( !QDir::home().remove( configFile ) )
        kdError(5500) << "Can't remove the note config: "
                      << configFile << endl;

    m_app->updateNoteActions();
}

int ResourceManager::count()
{
    return m_noteList.count();
}

QMap<QString,QString> ResourceManager::notes() const
{
    QMap<QString,QString> notes;
    QDictIterator<KNote> it( m_noteList );

    for ( ; it.current(); ++it )
        notes.insert( it.current()->noteId(), it.current()->name() );

    return notes;
}

void ResourceManager::sync( const QString& app )
{
    QDictIterator<KNote> it( m_noteList );

    for ( ; it.current(); ++it )
        it.current()->sync( app );
}


KNote* ResourceManager::first()
{
    QDictIterator<KNote> it( m_noteList );
    return it.toFirst();
}

void ResourceManager::showNextNote()
{
    // show next note
    QDictIterator<KNote> it( m_noteList );
    KNote* first = it.toFirst();
    for ( ; it.current(); ++it )
        if ( it.current()->hasFocus() )
        {
            if ( ++it )
                m_app->showNote( it.current() );
            else
                m_app->showNote( first );
            break;
        }
}

KNote* ResourceManager::note( const QString& id )
{
    return m_noteList[id];
}

void ResourceManager::resourceAdded( ResourceNotes* resource )
{
  kdDebug() << "Resource added: " << resource->resourceName() << endl;

  if ( !resource->isActive() ) return;

  resource->setManager( this );
  if ( resource->open() ) {
    resource->load();
  }
}

void ResourceManager::resourceModified( ResourceNotes* /*resource*/ )
{
}

void ResourceManager::resourceDeleted( ResourceNotes* /*resource*/ )
{
}


#include "resourcemanager.moc"
