

#ifndef KNOTE_H
#define KNOTE_H

#include "knoteedit.h"
#include "knotebutton.h"

#include <ksimpleconfig.h>
#include <kpopupmenu.h>
#include <qframe.h>
#include <qlabel.h>
#include <qdir.h>


class KNote : public QFrame  {
   Q_OBJECT
public:
    KNote( KSimpleConfig* config, QWidget* parent=0, const char* name=0 );
    ~KNote();

    void save();
    QString getName();
    void setOnDesktop( uint id );


public slots:
    //menu slots
    void slotMail   ( int );
    void slotPrint  ( int );
    void slotRename ( int );
    void slotInsDate( int );
    void slotConfig ( int );
    void slotKill   ( int );
    void slotClose  ();

    void slotApplyConfig();
    void slotToDesktop( int id );
    void slotPrepareDesktopMenu();
    void slotAlwaysOnTop( int );

    virtual void setFocus();
    virtual void show();

signals:
    void sigKilled( QString );
    void sigRenamed( QString&, QString& );
    void sigNewNote( int );

protected:
    void resizeEvent( QResizeEvent* );
    bool eventFilter( QObject* o, QEvent* e );

    KNoteEdit*     m_editor;
    QString        m_title;
    KSimpleConfig* m_config;
    KNoteButton*   m_button;
    QLabel*        m_label;
    KPopupMenu*    m_menu;
    KPopupMenu*    m_desktop_menu;
    int            m_headerHeight;

    bool           m_dragging;
    QPoint         m_pointerOffset;
    int            m_id;
    QDir*          m_notedir;

    int            m_idAlwaysOnTop;

};


#endif
