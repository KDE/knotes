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

#include <QDomDocument>
#include <QList>
#include <QMap>
#include <QString>
#include <QWidget>

#include <kapplication.h>
#include <ksessionmanager.h>
#include <kxmlguiclient.h>

class QTcpServer;

class KAction;
class KFind;
class KMenu;
class KNote;
class KNotesAlarm;
class KNotesResourceManager;
class KXMLGUIBuilder;
class KXMLGUIFactory;
class KNotesTray;

namespace KCal {
class Journal;
}
namespace DNSSD {
class PublicService;
}

class KNotesApp
        : public QWidget, public KSessionManager, virtual public KXMLGUIClient
{
    Q_OBJECT
public:
    KNotesApp();
    ~KNotesApp();

    void showNote( const QString &id ) const;
    void hideNote( const QString &id ) const;

    void killNote( const QString &id );
    void killNote( const QString &id, bool force );

    QString name( const QString &id ) const;
    QString text( const QString &id ) const;

    void setName( const QString &id, const QString &newName );
    void setText( const QString &id, const QString &newText );

    QVariantMap notes() const;

    bool commitData( QSessionManager & );

public slots:
    QString newNote( const QString &name = QString(),
                     const QString &text = QString() );
    QString newNoteFromClipboard( const QString &name = QString() );

    void hideAllNotes() const;
    void showAllNotes() const;

protected slots:
    void slotActivateRequested( bool, const QPoint& pos);
    void slotSecondaryActivateRequested( const QPoint& );
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
    void slotConfigUpdated();
    void acceptConnection();
    void saveNotes();
    void saveNotes( const QString & uid );
    void updateNoteActions();

    void createNote( KCal::Journal *journal );
    void killNote( KCal::Journal *journal );
    void slotPrintSelectedNotes();

private:
    void updateNetworkListener();
    QMap<QString, KNote *> m_notes;
    QList<QAction *>       m_noteActions;

    KNotesResourceManager  *m_manager;
    KNotesAlarm            *m_alarm;
    QTcpServer             *m_listener;
    DNSSD::PublicService   *m_publisher;

    KFind           *m_find;
    QMap<QString, KNote *>::iterator *m_findPos;

    KMenu           *m_noteMenu;
    KMenu           *m_contextMenu;

    KXMLGUIFactory  *m_guiFactory;
    KXMLGUIBuilder  *m_guiBuilder;
    KNotesTray *m_tray;
    KAction         *m_findAction;

    QDomDocument    m_noteGUI;
    QString m_noteUidModify;
};

#endif
