/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (C) Bernd Johannes Wuebben
     wuebben@math.cornell.edu
     wuebben@kde.org

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

#include "knote.h"
#include "knotesdcop.h"

#include <ksystemtray.h>
#include <kconfig.h>

#include <qdict.h>

class KNotesApp : public KSystemTray, virtual public KNotesDCOP
{
   Q_OBJECT
public:
    KNotesApp();
    ~KNotesApp();

	virtual ASYNC showNote( const QString& );
	virtual ASYNC addNote( QString, QString, unsigned long );
	virtual ASYNC rereadNotesDir();

public slots:
    void slotNewNote    ( int id=0 );
    void slotPreferences( int );

    void slotNoteKilled ( QString name );
    void slotNoteRenamed( QString& oldname, QString& newname );
    void slotSaveNotes();

protected slots:
    void slotToNote( int id );
    void slotPrepareNoteMenu();
    void mouseReleaseEvent( QMouseEvent *);

private:
    QDict<KNote>  m_NoteList;
    KPopupMenu*   m_note_menu;

    void loadNotes();
    void newNote( const QString& note_name = QString::null,
                  const QString& text = QString::null );
};

#endif
