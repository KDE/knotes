/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2006, The KNotes Developers

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

#ifndef KNOTESAPP_H
#define KNOTESAPP_H

#include <qstring.h>
#include <qdict.h>
#include <qptrlist.h>
#include <qlabel.h>
#include <qdom.h>

#include <kapplication.h>
#include <kxmlguiclient.h>

#include "KNotesAppIface.h"

class KFind;
class KNote;
class KPopupMenu;
class KAction;
class KActionMenu;
class KGlobalAccel;
class KXMLGUIFactory;
class KXMLGUIBuilder;
class KNotesAlarm;
class KNotesResourceManager;

namespace KCal {
    class Journal;
}

namespace KNetwork {
    class KServerSocket;
}


class KNotesApp : public QLabel, public KSessionManaged, virtual public KXMLGUIClient,
    virtual public KNotesAppIface
{
    Q_OBJECT
public:
    KNotesApp();
    ~KNotesApp();

    void showNote( const QString& id ) const;
    void hideNote( const QString& id ) const;

    void killNote( const QString& id );
    void killNote( const QString& id, bool force );

    QString name( const QString& id ) const;
    QString text( const QString& id ) const;

    void setName( const QString& id, const QString& newName );
    void setText( const QString& id, const QString& newText );

    QString fgColor( const QString& id ) const;
    QString bgColor( const QString& id ) const;

    void setColor( const QString& id, const QString& fgColor,
                                      const QString& bgColor );

    QMap<QString,QString> notes() const;

    int width( const QString& noteId ) const;
    int height( const QString& noteId ) const;

    void move( const QString& noteId, int x, int y ) const;
    void resize( const QString& noteId, int width, int height ) const;

    void sync( const QString& app );
    bool isNew( const QString& app, const QString& id ) const;
    bool isModified( const QString& app, const QString& id ) const;

    bool commitData( QSessionManager& );

public slots:
    QString newNote( const QString& name = QString::null,
                     const QString& text = QString::null );
    QString newNoteFromClipboard( const QString& name = QString::null );

    void hideAllNotes() const;
    void showAllNotes() const;

protected:
    void mousePressEvent( QMouseEvent* );

protected slots:
    void slotShowNote();
    void slotWalkThroughNotes();

    void slotOpenFindDialog();
    void slotFindNext();

    void slotPreferences();
    void slotConfigureAccels();

    void slotNoteKilled( KCal::Journal *journal );

    void slotQuit();

private:
    void showNote( KNote *note ) const;
    void saveConfigs();

private slots:
    void acceptConnection();
    void saveNotes();
    void saveNotes( const QString & uid );
    void updateNoteActions();
    void updateGlobalAccels();
    void updateNetworkListener();
    void updateStyle();

    void createNote( KCal::Journal *journal );
    void killNote( KCal::Journal *journal );

private:
    class KNoteActionList : public QPtrList<KAction>
    {
    public:
        virtual int compareItems( QPtrCollection::Item s1, QPtrCollection::Item s2 );
    };

    KNotesResourceManager *m_manager;

    KNotesAlarm     *m_alarm;
    KNetwork::KServerSocket   *m_listener;

    QDict<KNote>    m_noteList;
    KNoteActionList m_noteActions;

    KFind           *m_find;
    QDictIterator<KNote> *m_findPos;

    KPopupMenu      *m_note_menu;
    KPopupMenu      *m_context_menu;

    KGlobalAccel    *m_globalAccel;
    KXMLGUIFactory  *m_guiFactory;
    KXMLGUIBuilder  *m_guiBuilder;

    QDomDocument    m_noteGUI;
    KAction         *m_findAction;
    QString m_noteUidModify;
};

#endif
