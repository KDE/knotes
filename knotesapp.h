/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2004, The KNotes Developers

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
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

class KNote;
class KPopupMenu;
class KAction;
class KActionMenu;
class KGlobalAccel;
class KXMLGUIFactory;
class KXMLGUIBuilder;
class KExtendedSocket;
class KNotesResourceManager;

namespace KCal {
    class Journal;
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

    QMap<QString,QString> notes() const;

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
    void updateNoteActions();
    void updateGlobalAccels();
    void updateNetworkListener();

    void createNote( KCal::Journal *journal );
    void killNote( KCal::Journal *journal );

private:
    class KNoteActionList : public QPtrList<KAction>
    {
    public:
        virtual int compareItems( QPtrCollection::Item s1, QPtrCollection::Item s2 );
    };

    KNotesResourceManager *m_manager;

    KExtendedSocket *m_listener;

    QDict<KNote>    m_noteList;
    KNoteActionList m_noteActions;

    KPopupMenu      *m_note_menu;
    KPopupMenu      *m_context_menu;

    KGlobalAccel    *m_globalAccel;
    KXMLGUIFactory  *m_guiFactory;
    KXMLGUIBuilder  *m_guiBuilder;

    QDomDocument    m_noteGUI;
};

#endif
