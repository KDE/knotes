

#ifndef KNOTESAPP_H
#define KNOTESAPP_H

#include "knote.h"
#include <ksystemtray.h>
#include <kconfig.h>
#include <qdict.h>


class KNotesApp : public KSystemTray {
   Q_OBJECT
public:
    KNotesApp();
    ~KNotesApp();

public slots:
    void slotNewNote    ( int id=0 );
    void slotPreferences( int );

    void slotNoteKilled ( QString name );
    void slotNoteRenamed( QString& oldname, QString& newname );

protected slots:
    void slotToNote( int id );
    void slotPrepareNoteMenu();
    void mouseReleaseEvent( QMouseEvent *);

private:
    QDict< KNote >  m_NoteList;
    KPopupMenu*     m_note_menu;
    KConfig*        m_defaults;

    void copyDefaultConfig( KSimpleConfig* );
};

#endif
