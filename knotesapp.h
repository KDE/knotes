/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2001, The KNotes Developers

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

#ifndef KNOTESAPP_H
#define KNOTESAPP_H

#include <qstring.h>
#include <qdict.h>

#include <ksystemtray.h>

#include "KNotesIface.h"

class KNote;


class KNotesApp : public KSystemTray, virtual public KNotesIface
{
    Q_OBJECT
public:
    KNotesApp();
    ~KNotesApp();

    int newNote( QString name = QString::null, const QString& text = QString::null );
    void showNote( const QString& name ) const;
    void showNote( int noteId ) const;
    void killNote( const QString& name );
    void killNote( int noteId );

    QMap<int,QString> notes() const;
    QString text( const QString& name ) const;
    QString text( int noteId ) const;
    void setName( const QString& oldName, const QString& newName );
    void setName( int noteId, const QString& newName );
    void setText( const QString& name, const QString& newText );
    void setText( int noteId, const QString& newText );

    void sync( const QString& app );
    bool isNew( const QString& app, const QString& name ) const;
    bool isNew( const QString& app, int noteId ) const;
    bool isModified( const QString& app, const QString& name ) const;
    bool isModified( const QString& app, int noteId ) const;

protected:
    void mouseReleaseEvent( QMouseEvent* );

protected slots:
    void slotNewNote();
    void slotToNote( int id ) const;
    void slotSaveNotes() const;

    void slotPreferences() const;
    void slotPrepareNoteMenu();

    void slotNoteKilled( const QString& name );
    void slotNoteRenamed( const QString& oldname, const QString& newname );

private:
    KNote* noteById( int id ) const;

    QDict<KNote>  m_NoteList;
    KPopupMenu*   m_note_menu;
};

#endif
