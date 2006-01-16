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

#include <QClipboard>
#include <QMouseEvent>
#include <QToolTip>
#include <QPixmap>
#include <QLabel>
#include <QX11Info>

#include <kdebug.h>
#include <kaction.h>
#include <kxmlguifactory.h>
#include <kxmlguibuilder.h>
#include <ksystemtray.h>
#include <klocale.h>
#include <kiconeffect.h>
#include <kstandarddirs.h>
#include <kmenu.h>
#include <khelpmenu.h>
#include <kfind.h>
#include <kfinddialog.h>
#include <kkeydialog.h>
#include <kglobalaccel.h>
#include <ksimpleconfig.h>
#include <kwin.h>
#include <kbufferedsocket.h>
#include <kserversocket.h>

#include <libkcal/journal.h>
#include <libkcal/calendarlocal.h>
#include <kiconloader.h>

#include "knotesapp.h"
#include "knote.h"
#include "knotesalarm.h"
#include "knoteconfigdlg.h"
#include "knotesglobalconfig.h"
#include "knoteslegacy.h"
#include "knotesnetrecv.h"

#include "knotes/resourcemanager.h"

using namespace KNetwork;


class KNotesKeyDialog : public KDialogBase
{
public:
    KNotesKeyDialog( KGlobalAccel *globals, QWidget *parent, const char* name = 0 )
        : KDialogBase( parent, name, true, i18n("Configure Shortcuts"), Default|Ok|Cancel, Ok )
    {
        m_keyChooser = new KKeyChooser( globals, this );
        setMainWidget( m_keyChooser );
        connect( this, SIGNAL(defaultClicked()), m_keyChooser, SLOT(allDefault()) );
    }

    void insert( KActionCollection *actions )
    {
        m_keyChooser->insert( actions, i18n("Note Actions") );
    }

    void configure()
    {
        if ( exec() == Accepted )
            m_keyChooser->save();
    }

private:
    KKeyChooser *m_keyChooser;
};


static bool kActionLessThan( const KAction *a1, const KAction *a2 )
{
    return a1->text() < a2->text();
}


KNotesApp::KNotesApp()
    : DCOPObject("KNotesIface"), QLabel( 0, Qt::Window ),
      m_alarm( 0 ), /*m_listener( 0 ),*/ m_find( 0 ), m_findPos( 0 )
{
    connect( kapp, SIGNAL(lastWindowClosed()), kapp, SLOT(quit()) );
    
    // create the dock widget...
    KWin::setSystemTrayWindowFor( winId(), QX11Info::appRootWindow() );
    setToolTip( i18n("KNotes: Sticky notes for KDE") );
    setPixmap( KSystemTray::loadIcon( "knotes" ) );

    // set the initial style
#warning FIXME
//    KNote::setStyle( KNotesGlobalConfig::style() );

    // create the GUI...
    new KAction( i18n("New Note"), "filenew", 0,
        this, SLOT(newNote()), actionCollection(), "new_note" );
    new KAction( i18n("New Note From Clipboard"), "editpaste", 0,
        this, SLOT(newNoteFromClipboard()), actionCollection(), "new_note_clipboard" );
    new KAction( i18n("Show All Notes"), "knotes", 0,
        this, SLOT(showAllNotes()), actionCollection(), "show_all_notes" );
    new KAction( i18n("Hide All Notes"), "fileclose", 0,
        this, SLOT(hideAllNotes()), actionCollection(), "hide_all_notes" );
    new KHelpMenu( this, kapp->aboutData(), false, actionCollection() );

    KStdAction::find( this, SLOT(slotOpenFindDialog()), actionCollection() );
    KStdAction::preferences( this, SLOT(slotPreferences()), actionCollection() );
    KStdAction::keyBindings( this, SLOT(slotConfigureAccels()), actionCollection() );
    //FIXME: no shortcut removing!?
    KStdAction::quit( this, SLOT(slotQuit()), actionCollection() )->setShortcut( 0 );

    setXMLFile( instance()->instanceName() + "appui.rc" );

    m_guiBuilder = new KXMLGUIBuilder( this );
    m_guiFactory = new KXMLGUIFactory( m_guiBuilder, this );
    m_guiFactory->addClient( this );

    m_contextMenu = static_cast<KMenu*>(m_guiFactory->container( "knotes_context", this ));
    m_noteMenu = static_cast<KMenu*>(m_guiFactory->container( "notes_menu", this ));

    // get the most recent XML UI file
    QString xmlFileName = instance()->instanceName() + "ui.rc";
    QString filter = QString::fromLatin1( instance()->instanceName() + '/' ) + xmlFileName;
    QStringList fileList = instance()->dirs()->findAllResources( "data", filter ) +
                           instance()->dirs()->findAllResources( "data", xmlFileName );

    QString doc;
    KXMLGUIClient::findMostRecentXMLFile( fileList, doc );
    m_noteGUI.setContent( doc );

    // create accels for global shortcuts
    m_globalAccel = new KGlobalAccel( this );
    m_globalAccel->setObjectName( "global accel" );
    m_globalAccel->insert( "global_new_note", i18n("New Note"), "",
                           Qt::ALT+Qt::SHIFT+Qt::Key_N,
                           this, SLOT(newNote()), true, true );
    m_globalAccel->insert( "global_new_note_clipboard", i18n("New Note From Clipboard"), "",
                           Qt::ALT+Qt::SHIFT+Qt::Key_C,
                           this, SLOT(newNoteFromClipboard()), true, true );
    m_globalAccel->insert( "global_hide_all_notes", i18n("Hide All Notes"), "",
                           Qt::ALT+Qt::SHIFT+Qt::Key_H,
                           this, SLOT(hideAllNotes()), true, true );
    m_globalAccel->insert( "global_show_all_notes", i18n("Show All Notes"), "",
                           Qt::ALT+Qt::SHIFT+Qt::Key_S,
                           this, SLOT(showAllNotes()), true, true );

    m_globalAccel->readSettings();

    KConfig *config = KGlobal::config();
    config->setGroup( "Global Keybindings" );
    m_globalAccel->setEnabled( config->readEntry( "Enabled", true ) );

    updateGlobalAccels();

    // clean up old config files
    KNotesLegacy::cleanUp();

    // create the resource manager
    m_manager = new KNotesResourceManager();
    connect( m_manager, SIGNAL(sigRegisteredNote( KCal::Journal * )),
             this,      SLOT(createNote( KCal::Journal * )) );
    connect( m_manager, SIGNAL(sigDeregisteredNote( KCal::Journal * )),
             this,      SLOT(killNote( KCal::Journal * )) );

    // read the notes
    m_manager->load();

    // read the old config files, convert and add them
    KCal::CalendarLocal calendar( QString::fromLatin1( "UTC" ) );
    if ( KNotesLegacy::convert( &calendar ) )
    {
        KCal::Journal::List notes = calendar.journals();
        KCal::Journal::List::ConstIterator it;
        for ( it = notes.begin(); it != notes.end(); ++it )
            m_manager->addNewNote( *it );

        m_manager->save();
    }

    // set up the alarm reminder - do it after loading the notes because this
    // is used as a check if updateNoteActions has to be called for a new note
    m_alarm = new KNotesAlarm( m_manager, this );

    // create the socket and possibly start listening for connections
    m_listener = new KServerSocket();
    m_listener->setResolutionEnabled( true );
    connect( m_listener, SIGNAL(readyAccept()), SLOT(acceptConnection()) );
    updateNetworkListener();

    if ( m_notes.size() == 0 && !kapp->isSessionRestored() )
        newNote();

    updateNoteActions();
}

KNotesApp::~KNotesApp()
{
    saveNotes();

    blockSignals( true );
    qDeleteAll( m_notes );
    m_notes.clear();
    qDeleteAll( m_noteActions );
    m_noteActions.clear();
    blockSignals( false );

    //delete m_listener;
    delete m_manager;
    delete m_guiBuilder;
}

bool KNotesApp::commitData( QSessionManager& )
{
    saveConfigs();
    return true;
}

// -------------------- public DCOP interface -------------------- //

QString KNotesApp::newNote( const QString& name, const QString& text )
{
    // create the new note
    KCal::Journal *journal = new KCal::Journal();

    // new notes have the current date/time as title if none was given
    if ( !name.isEmpty() )
        journal->setSummary( name );
    else
        journal->setSummary( KGlobal::locale()->formatDateTime( QDateTime::currentDateTime() ) );

    // the body of the note
    journal->setDescription( text );

    m_manager->addNewNote( journal );

    showNote( journal->uid() );

    return journal->uid();
}

QString KNotesApp::newNoteFromClipboard( const QString& name )
{
    const QString& text = KApplication::clipboard()->text();
    return newNote( name, text );
}

void KNotesApp::hideAllNotes() const
{
    foreach ( KNote *note, m_notes )
        note->close();
}

void KNotesApp::showAllNotes() const
{
    foreach ( KNote *note, m_notes )
    {
        note->show();
        note->setFocus();
    }
}

void KNotesApp::showNote( const QString& id ) const
{
    KNote *note = m_notes.value( id );
    if ( note )
        showNote( note );
    else
        kdWarning(5500) << "showNote: no note with id: " << id << endl;
}

void KNotesApp::hideNote( const QString& id ) const
{
    KNote *note = m_notes.value( id );
    if ( note )
        note->hide();
    else
        kdWarning(5500) << "hideNote: no note with id: " << id << endl;
}

void KNotesApp::killNote( const QString& id, bool force )
{
    KNote *note = m_notes.value( id );
    if ( note )
        note->slotKill( force );
    else
        kdWarning(5500) << "killNote: no note with id: " << id << endl;
}

// "bool force = false" doesn't work with dcop
void KNotesApp::killNote( const QString& id )
{
    killNote( id, false );
}

QMap<QString, QString> KNotesApp::notes() const
{
    QMap<QString, QString> notes;

    foreach ( KNote *note, m_notes )
        notes.insert( note->noteId(), note->name() );

    return notes;
}

QString KNotesApp::name( const QString& id ) const
{
    KNote *note = m_notes.value( id );
    if ( note )
        return note->name();
    else
        return QString();
}

QString KNotesApp::text( const QString& id ) const
{
    KNote *note = m_notes.value( id );
    if ( note )
        return note->text();
    else
        return QString();
}

void KNotesApp::setName( const QString& id, const QString& newName )
{
    KNote *note = m_notes.value( id );
    if ( note )
        note->setName( newName );
    else
        kdWarning(5500) << "setName: no note with id: " << id << endl;
}

void KNotesApp::setText( const QString& id, const QString& newText )
{
    KNote *note = m_notes.value( id );
    if ( note )
        note->setText( newText );
    else
        kdWarning(5500) << "setText: no note with id: " << id << endl;
}


// ------------------- protected methods ------------------- //

void KNotesApp::mousePressEvent( QMouseEvent *e )
{
    if ( !rect().contains( e->pos() ) )
        return;

    switch ( e->button() )
    {
    case Qt::LeftButton:
        if ( m_notes.size() == 1 )
            showNote( *m_notes.begin() );
        else if ( m_notes.size() > 1 )
            m_noteMenu->popup( e->globalPos() );
        break;
    case Qt::MidButton:
        newNote();
        break;
    case Qt::RightButton:
        m_contextMenu->popup( e->globalPos() );
    default: break;
    }
}

// -------------------- protected slots -------------------- //

void KNotesApp::slotShowNote()
{
    // tell the WM to give this note focus
    showNote( sender()->objectName() );
}

void KNotesApp::slotWalkThroughNotes()
{
    // show next note
    QMap<QString, KNote *>::const_iterator it = m_notes.begin();
    for ( ; it != m_notes.end(); ++it )
        if ( (*it)->hasFocus() )
        {
            if ( ++it != m_notes.end() )
                showNote( *it );
            else
                showNote( *m_notes.begin() );
            break;
        }
}

void KNotesApp::slotOpenFindDialog()
{
    KFindDialog findDia( this, "find_dialog" );
    findDia.setHasSelection( false );
    findDia.setHasCursor( false );
    findDia.setSupportsBackwardsFind( false );

    if ( findDia.exec() != QDialog::Accepted )
        return;

    delete m_findPos;
    m_findPos = new QMap<QString, KNote *>::iterator();
    *m_findPos = m_notes.begin();

    // this could be in an own method if searching without a dialog should be possible
    delete m_find;
    m_find = new KFind( findDia.pattern(), findDia.options(), this );

    slotFindNext();
}

void KNotesApp::slotFindNext()
{
    if ( *m_findPos != m_notes.end() )
    {
        KNote *note = *(*m_findPos++);
        note->find( m_find->pattern(), m_find->options() );
    }
    else
    {
        m_find->displayFinalDialog();
        delete m_find;
        m_find = 0;
        delete m_findPos;
        m_findPos = 0;
    }
}

void KNotesApp::slotPreferences()
{
    // reuse the dialog if possible
    if ( KNoteConfigDlg::showDialog( "KNotes Default Settings" ) )
        return;

    // create a new preferences dialog...
    KNoteConfigDlg *dialog = new KNoteConfigDlg( 0, i18n("Settings"), this,
                                                 "KNotes Settings" );
    connect( dialog, SIGNAL(settingsChanged()), this, SLOT(updateNetworkListener()) );
    connect( dialog, SIGNAL(settingsChanged()), this, SLOT(updateStyle()) );
    dialog->show();
}

void KNotesApp::slotConfigureAccels()
{
    KNotesKeyDialog keys( m_globalAccel, this );
    QMap<QString, KNote *>::const_iterator it = m_notes.begin();
    if ( !m_notes.isEmpty() )
        keys.insert( (*it)->actionCollection() );
    keys.configure();

    m_globalAccel->writeSettings();
    updateGlobalAccels();

    // update GUI doc for new notes
    m_noteGUI.setContent(
        KXMLGUIFactory::readConfigFile( instance()->instanceName() + "ui.rc", instance() )
    );

    if ( m_notes.isEmpty() )
        return;

    QList<KAction *> list = (*it)->actionCollection()->actions();
    foreach ( KAction *action, list )
    {
        it = m_notes.begin();
        for ( ++it; it != m_notes.end(); ++it )
        {
#warning Port KAction::action() to QString
            KAction *toChange = (*it)->actionCollection()->action( action->objectName().toUtf8().data() );
            if ( toChange->shortcut() != action->shortcut() )
                toChange->setShortcut( action->shortcut() );
        }
    }
}

void KNotesApp::slotNoteKilled( KCal::Journal *journal )
{
    m_manager->deleteNote( journal );
    saveNotes();
}

void KNotesApp::slotQuit()
{
    foreach ( KNote *note, m_notes )
        if ( note->isModified() )
            note->saveData();

    saveConfigs();
    kapp->quit();
}


// -------------------- private methods -------------------- //

void KNotesApp::showNote( KNote* note ) const
{
    note->show();
    KWin::setCurrentDesktop( KWin::windowInfo( note->winId() ).desktop() );
    KWin::forceActiveWindow( note->winId() );
    note->setFocus();
}

void KNotesApp::createNote( KCal::Journal *journal )
{
    KNote *newNote = new KNote( m_noteGUI, journal, 0 );
    m_notes.insert( newNote->noteId(), newNote );

    connect( newNote, SIGNAL(sigRequestNewNote()), SLOT(newNote()) );
    connect( newNote, SIGNAL(sigShowNextNote()), SLOT(slotWalkThroughNotes()) );
    connect( newNote, SIGNAL(sigKillNote( KCal::Journal* )),
                        SLOT(slotNoteKilled( KCal::Journal* )) );
    connect( newNote, SIGNAL(sigNameChanged()), SLOT(updateNoteActions()) );
    connect( newNote, SIGNAL(sigDataChanged()), SLOT(saveNotes()) );
    connect( newNote, SIGNAL(sigColorChanged()), SLOT(updateNoteActions()) );
    connect( newNote, SIGNAL(sigFindFinished()), SLOT(slotFindNext()) );

    // don't call this during startup for each and every loaded note
    if ( m_alarm )
        updateNoteActions();
}

void KNotesApp::killNote( KCal::Journal *journal )
{
    // this kills the KNote object
    delete m_notes.take( journal->uid() );
    updateNoteActions();
}

void KNotesApp::acceptConnection()
{
    // Accept the connection and make KNotesNetworkReceiver do the job
    KBufferedSocket *s = static_cast<KBufferedSocket *>(m_listener->accept());
    if ( s )
    {
        KNotesNetworkReceiver *recv = new KNotesNetworkReceiver( s );
        connect( recv, SIGNAL(sigNoteReceived( const QString &, const QString & )),
                 this, SLOT(newNote( const QString &, const QString & )) );
    }
}

void KNotesApp::saveNotes()
{
    KNotesGlobalConfig::writeConfig();
    m_manager->save();
}

void KNotesApp::saveConfigs()
{
    foreach ( KNote *note, m_notes )
        note->saveConfig();
}

void KNotesApp::updateNoteActions()
{
    unplugActionList( "notes" );
    m_noteActions.clear();

    foreach ( KNote *note, m_notes )
    {
#warning utf8: use QString
        KAction *action = new KAction( note->name().replace("&", "&&"),
                                       KShortcut(), this, SLOT(slotShowNote()),
                                       0, note->noteId().toUtf8() );
        KIconEffect effect;
        QPixmap icon = effect.apply( 
                qApp->windowIcon().pixmap( IconSize(KIcon::Small), IconSize(KIcon::Small) ),
                KIconEffect::Colorize, 1, note->palette().color( note->backgroundRole() ), false 
        );
        action->setIcon( icon );
        m_noteActions.append( action );
    }

	qSort( m_noteActions.begin(), m_noteActions.end(), kActionLessThan );

    if ( m_noteActions.isEmpty() )
    {
        KAction *action = new KAction( i18n("No Notes"), KShortcut::null(), 0, 0, 0, 0 );
        m_noteActions.append( action );
    }

    plugActionList( "notes", m_noteActions );
}

void KNotesApp::updateGlobalAccels()
{
    if ( m_globalAccel->isEnabled() )
    {
        KAction *action = actionCollection()->action( "new_note" );
        if ( action )
            action->setShortcut( m_globalAccel->shortcut( "global_new_note" ) );
        action = actionCollection()->action( "new_note_clipboard" );
        if ( action )
            action->setShortcut( m_globalAccel->shortcut( "global_new_note_clipboard" ) );
        action = actionCollection()->action( "hide_all_notes" );
        if ( action )
            action->setShortcut( m_globalAccel->shortcut( "global_hide_all_notes" ) );
        action = actionCollection()->action( "show_all_notes" );
        if ( action )
            action->setShortcut( m_globalAccel->shortcut( "global_show_all_notes" ) );

        m_globalAccel->updateConnections();
    }
    else
    {
        KAction *action = actionCollection()->action( "new_note" );
        if ( action )
            action->setShortcut( 0 );
        action = actionCollection()->action( "new_note_clipboard" );
        if ( action )
            action->setShortcut( 0 );
        action = actionCollection()->action( "hide_all_notes" );
        if ( action )
            action->setShortcut( 0 );
        action = actionCollection()->action( "show_all_notes" );
        if ( action )
            action->setShortcut( 0 );
    }
}

void KNotesApp::updateNetworkListener()
{
    m_listener->close();

    if ( KNotesGlobalConfig::receiveNotes() )
    {
        m_listener->setAddress( QString::number( KNotesGlobalConfig::port() ) );
        m_listener->bind();
        m_listener->listen();
    }
}

void KNotesApp::updateStyle()
{
#warning FIXME!
//    KNote::setStyle( KNotesGlobalConfig::style() );

    foreach ( KNote *note, m_notes )
        QApplication::postEvent( note, new QEvent( QEvent::LayoutHint ) );
}

#include "knotesapp.moc"
