/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2009, The KNotes Developers

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

#include "knoteconfigdlg.h"
#include "knote.h"
#include "knotes/resourcemanager.h"
#include "knotesadaptor.h"
#include "knotesalarm.h"
#include "knotesapp.h"
#include "knotesglobalconfig.h"
#include "knoteslegacy.h"
#include "knotesnetrecv.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfind.h>
#include <kfinddialog.h>
#include <khelpmenu.h>
#include <kicon.h>
#include <kiconeffect.h>
#include <klocale.h>
#include <kmenu.h>
#include <kshortcutsdialog.h>
#include <ksocketfactory.h>
#include <kstandardaction.h>
#include <kstandarddirs.h>
#include <ksystemtrayicon.h>
#include <kwindowsystem.h>
#include <kxmlguibuilder.h>
#include <kxmlguifactory.h>
#include <KStatusNotifierItem>

#include <kcal/calendarlocal.h>
#include <kcal/journal.h>
#include <kiconloader.h>

#include <QPixmap>
#include <QClipboard>
#include <QTcpServer>

#include <dnssd/publicservice.h>


class KNotesKeyDialog
  : public KDialog
{
  public:
    KNotesKeyDialog( KActionCollection *globals, QWidget *parent )
      : KDialog( parent )
    {
      setCaption( i18n( "Configure Shortcuts" ) );
      setButtons( Default | Ok | Cancel );

      m_keyChooser = new KShortcutsEditor( globals, this );
      setMainWidget( m_keyChooser );
      connect( this, SIGNAL(defaultClicked()),
               m_keyChooser, SLOT(allDefault()) );
    }

    void insert( KActionCollection *actions )
    {
        m_keyChooser->addCollection( actions, i18n( "Note Actions" ) );
    }

    void configure()
    {
        if ( exec() == Accepted ) {
            m_keyChooser->save();
        }
    }

  private:
    KShortcutsEditor *m_keyChooser;
};


static bool qActionLessThan( const QAction *a1, const QAction *a2 )
{
  return ( a1->text() < a2->text() );
}


KNotesApp::KNotesApp()
  : QWidget(), m_alarm( 0 ), m_listener( 0 ), m_publisher( 0 ), m_find( 0 ), m_findPos( 0 )
{
  new KNotesAdaptor( this );
  QDBusConnection::sessionBus().registerObject( "/KNotes" , this );
  kapp->setQuitOnLastWindowClosed( false );

  // create the dock widget...
  m_tray = new KStatusNotifierItem(0);

  m_tray->setToolTipTitle( i18n( "KNotes: Sticky notes for KDE" ) );
  m_tray->setIconByName( "knotes" );
  m_tray->setToolTipIconByName( "knotes" );
  m_tray->setStatus( KStatusNotifierItem::Active );
  m_tray->setCategory( KStatusNotifierItem::ApplicationStatus );
  m_tray->setStandardActionsEnabled(false);
  connect( m_tray, SIGNAL(activateRequested(bool,QPoint)), this, SLOT(slotActivateRequested(bool,QPoint)) );
  connect( m_tray, SIGNAL(secondaryActivateRequested(QPoint)), this, SLOT(slotSecondaryActivateRequested(QPoint)) );

  // set the initial style
#ifdef __GNUC__
#warning FIXME
#endif
  //    KNote::setStyle( KNotesGlobalConfig::style() );

  // create the GUI...
  KAction *action  = new KAction( KIcon( "document-new" ),
                                  i18n( "New Note" ), this );
  actionCollection()->addAction( "new_note", action );
  action->setGlobalShortcut( KShortcut( Qt::ALT + Qt::SHIFT + Qt::Key_N ));
  connect( action, SIGNAL(triggered()), SLOT(newNote()) );

  action  = new KAction( KIcon( "edit-paste" ),
                         i18n( "New Note From Clipboard" ), this );
  actionCollection()->addAction( "new_note_clipboard", action );
  action->setGlobalShortcut( KShortcut( Qt::ALT + Qt::SHIFT + Qt::Key_C ));
  connect( action, SIGNAL(triggered()), SLOT(newNoteFromClipboard()) );

  action  = new KAction( KIcon( "knotes" ), i18n( "Show All Notes" ), this );
  actionCollection()->addAction( "show_all_notes", action );
  action->setGlobalShortcut( KShortcut( Qt::ALT + Qt::SHIFT + Qt::Key_S ));
  connect( action, SIGNAL(triggered()), SLOT(showAllNotes()) );

  action  = new KAction( KIcon( "window-close" ),
                         i18n( "Hide All Notes" ), this );
  actionCollection()->addAction( "hide_all_notes", action );
  action->setGlobalShortcut( KShortcut( Qt::ALT + Qt::SHIFT + Qt::Key_H ));
  connect( action, SIGNAL(triggered()), SLOT(hideAllNotes()) );

  new KHelpMenu( this, KGlobal::mainComponent().aboutData(), false,
                 actionCollection() );

  m_findAction = KStandardAction::find( this, SLOT(slotOpenFindDialog()),
                         actionCollection() );
  KStandardAction::preferences( this, SLOT(slotPreferences()),
                         actionCollection() );
  KStandardAction::keyBindings( this, SLOT(slotConfigureAccels()),
                         actionCollection() );
  //FIXME: no shortcut removing!?
  KStandardAction::quit( this, SLOT(slotQuit()),
                         actionCollection() )->setShortcut( 0 );

  setXMLFile( componentData().componentName() + "appui.rc" );

  m_guiBuilder = new KXMLGUIBuilder( this );
  m_guiFactory = new KXMLGUIFactory( m_guiBuilder, this );
  m_guiFactory->addClient( this );

  m_contextMenu = static_cast<KMenu *>( m_guiFactory->container(
                                        "knotes_context",
                                        this ) );
  m_noteMenu = static_cast<KMenu *>( m_guiFactory->container(
                                      "notes_menu", this ) );
  m_tray->setContextMenu( m_contextMenu );
  // get the most recent XML UI file
  QString xmlFileName = componentData().componentName() + "ui.rc";
  QString filter = componentData().componentName() + '/' + xmlFileName;
  const QStringList fileList =
      componentData().dirs()->findAllResources( "data", filter ) +
      componentData().dirs()->findAllResources( "data", xmlFileName );

  QString doc;
  KXMLGUIClient::findMostRecentXMLFile( fileList, doc );
  m_noteGUI.setContent( doc );

  KConfigGroup config( KGlobal::config(), "Global Keybindings" );

  // clean up old config files
  KNotesLegacy::cleanUp();

  // create the resource manager
  m_manager = new KNotesResourceManager();
  connect( m_manager, SIGNAL(sigRegisteredNote(KCal::Journal*)),
           this,      SLOT(createNote(KCal::Journal*)) );
  connect( m_manager, SIGNAL(sigDeregisteredNote(KCal::Journal*)),
           this,      SLOT(killNote(KCal::Journal*)) );

  // read the notes
  m_manager->load();

  // read the old config files, convert and add them
  KCal::CalendarLocal calendar( QString::fromLatin1( "UTC" ) );
  if ( KNotesLegacy::convert( &calendar ) ) {
    KCal::Journal::List notes = calendar.journals();
    KCal::Journal::List::ConstIterator it;
    for ( it = notes.constBegin(); it != notes.constEnd(); ++it ) {
      m_manager->addNewNote( *it );
    }

    m_manager->save();
  }

  // set up the alarm reminder - do it after loading the notes because this
  // is used as a check if updateNoteActions has to be called for a new note
  m_alarm = new KNotesAlarm( m_manager, this );

   updateNetworkListener();

  if ( m_notes.size() == 0 && !kapp->isSessionRestored() ) {
      newNote();
  }

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

  delete m_listener;
  m_listener=0;
  delete m_publisher;
  m_publisher=0;
  delete m_findPos;
  m_findPos = 0;
  //delete m_listener;
  delete m_manager;
  delete m_guiBuilder;
  delete m_tray;
}

bool KNotesApp::commitData( QSessionManager & )
{
  foreach ( KNote *note, m_notes ) {
    note->commitData();
  }
  saveConfigs();
  return true;
}

// -------------------- public D-Bus interface -------------------- //

QString KNotesApp::newNote( const QString &name, const QString &text )
{
  // create the new note
  KCal::Journal *journal = new KCal::Journal();

  // new notes have the current date/time as title if none was given
  if ( !name.isEmpty() ) {
    journal->setSummary( name );
  } else {
    journal->setSummary( KGlobal::locale()->formatDateTime(
                                               QDateTime::currentDateTime() ) );
  }

  // the body of the note
  journal->setDescription( text );

  if ( m_manager->addNewNote( journal ) ) {
    showNote( journal->uid() );
  }

  return journal->uid();
}

QString KNotesApp::newNoteFromClipboard( const QString &name )
{
  const QString &text = KApplication::clipboard()->text();
  return newNote( name, text );
}

void KNotesApp::hideAllNotes() const
{
  foreach ( KNote *note, m_notes ) {
    note->slotClose();
  }
}

void KNotesApp::showAllNotes() const
{
  foreach ( KNote *note, m_notes ) {
    // workaround to BUG 149116
    note->hide();

    note->show();
  }
}

void KNotesApp::showNote( const QString &id ) const
{
  KNote *note = m_notes.value( id );
  if ( note ) {
    showNote( note );
  } else {
    kWarning( 5500 ) << "showNote: no note with id:" << id;
  }
}

void KNotesApp::hideNote( const QString &id ) const
{
  KNote *note = m_notes.value( id );
  if ( note ) {
    note->hide();
  } else {
    kWarning( 5500 ) << "hideNote: no note with id:" << id;
  }
}

void KNotesApp::killNote( const QString &id, bool force )
{
  KNote *note = m_notes.value( id );
  if ( note ) {
    note->slotKill( force );
  } else {
    kWarning( 5500 ) << "killNote: no note with id:" << id;
  }
}

// "bool force = false" doesn't work with dcop
void KNotesApp::killNote( const QString &id )
{
  killNote( id, false );
}

QVariantMap KNotesApp::notes() const
{
  QVariantMap notes;

  foreach ( KNote *note, m_notes ) {
    notes.insert( note->noteId(), note->name() );
  }

  return notes;
}

QString KNotesApp::name( const QString &id ) const
{
  KNote *note = m_notes.value( id );
  if ( note ) {
    return note->name();
  } else {
    return QString();
  }
}

QString KNotesApp::text( const QString &id ) const
{
  KNote *note = m_notes.value( id );
  if ( note ) {
    return note->text();
  } else {
    return QString();
  }
}

void KNotesApp::setName( const QString &id, const QString &newName )
{
  KNote *note = m_notes.value( id );
  if ( note ) {
    note->setName( newName );
  } else {
    kWarning( 5500 ) << "setName: no note with id:" << id;
  }
}

void KNotesApp::setText( const QString &id, const QString &newText )
{
  KNote *note = m_notes.value( id );
  if ( note ) {
    note->setText( newText );
  } else {
    kWarning( 5500 ) << "setText: no note with id:" << id;
  }
}

// -------------------- protected slots -------------------- //

void KNotesApp::slotActivateRequested( bool, const QPoint&)
{
    if ( m_notes.size() == 1 ) {
        showNote( *m_notes.begin() );
    } else if ( m_notes.size() != 1 ) {
        m_noteMenu->popup( QCursor::pos ());
    }
}

void KNotesApp::slotSecondaryActivateRequested( const QPoint & )
{
    newNote();
}

void KNotesApp::slotShowNote()
{
  // tell the WM to give this note focus
  showNote( sender()->objectName() );
}

void KNotesApp::slotWalkThroughNotes()
{
  // show next note
  QMap<QString, KNote *>::const_iterator it = m_notes.constBegin();
  for ( ; it != m_notes.constEnd(); ++it ) {
    if ( ( *it )->hasFocus() ) {
      if ( ++it != m_notes.constEnd() ) {
        showNote( *it );
      } else {
        showNote( *m_notes.constBegin() );
      }
      break;
    }
  }
}

void KNotesApp::slotOpenFindDialog()
{
  KFindDialog findDia( this );
  findDia.setObjectName( "find_dialog" );
  findDia.setHasSelection( false );
  findDia.setHasCursor( false );
  findDia.setSupportsBackwardsFind( false );

  if ( (findDia.exec() != QDialog::Accepted) ) {
      delete m_findPos;
      m_findPos = 0;
      delete m_find;
      m_find = 0;
      return;
  }

  delete m_findPos;
  m_findPos = new QMap<QString, KNote *>::iterator();
  *m_findPos = m_notes.begin();

  // this could be in an own method if searching without a dialog
  // should be possible
  delete m_find;
  m_find = new KFind( findDia.pattern(), findDia.options(), this );

  slotFindNext();
}

void KNotesApp::slotFindNext()
{
  if ( *m_findPos != m_notes.end() ) {
    KNote *note = * ( (*m_findPos)++ );
    note->find( m_find );
  } else {
    m_find->displayFinalDialog();
    m_find->deleteLater(); //we can't delete m_find now because it is the signal emitter
    m_find = 0;
    delete m_findPos;
    m_findPos = 0;
  }
}

void KNotesApp::slotPreferences()
{
  // create a new preferences dialog...
  KNoteConfigDlg *dialog = new KNoteConfigDlg( i18n( "Settings" ), this);
  connect( dialog, SIGNAL(configWrote()),
           this,   SLOT(updateNetworkListener()) );
  connect( dialog, SIGNAL(configWrote()),
           this,   SLOT(updateStyle()) );
  dialog->show();
}

void KNotesApp::slotConfigureAccels()
{
  KNotesKeyDialog keys( actionCollection(), this );

  QMap<QString, KNote *>::const_iterator it = m_notes.constBegin();

  if ( !m_notes.isEmpty() ) {
    keys.insert( ( *it )->actionCollection() );
  }

  keys.configure();

  // update GUI doc for new notes
  m_noteGUI.setContent(
    KXMLGUIFactory::readConfigFile( componentData().componentName() + "ui.rc",
                                    componentData() )
                      );

  if ( m_notes.isEmpty() ) {
    return;
  }

  foreach ( QAction *action, ( *it )->actionCollection()->actions() ) {
    it = m_notes.constBegin();
    for ( ++it; it != m_notes.constEnd(); ++it ) {
/*
    // Not sure if this is what this message has in mind but since both
    // action->objectName() and KAction::action() are QStrings, this
    // might be fine.
    // Correct me if I am wrong... ~ gamaral
#ifdef __GNUC__
#warning Port KAction::action() to QString
#endif
*/
      QAction *toChange =
        ( *it )->actionCollection()->action( action->objectName() );

      if ( toChange ) {
        toChange->setShortcuts( action->shortcuts() );
      }
    }
  }
}

void KNotesApp::slotNoteKilled( KCal::Journal *journal )
{
  m_noteUidModify.clear();
  m_manager->deleteNote( journal );
  saveNotes();
}

void KNotesApp::slotQuit()
{
  foreach ( KNote *note, m_notes ) {
    if ( note->isModified() ) {
      note->saveData(false);
    }
  }
  saveConfigs();
  kapp->quit();
}

// -------------------- private methods -------------------- //

void KNotesApp::showNote( KNote *note ) const
{
  note->show();
#ifdef Q_WS_X11
  if ( !note->isDesktopAssigned() ) {
    note->toDesktop( KWindowSystem::currentDesktop() );
  } else {
    KWindowSystem::setCurrentDesktop(
      KWindowSystem::windowInfo( note->winId(), NET::WMDesktop ).desktop() );
  }
  KWindowSystem::forceActiveWindow( note->winId() );
#endif
  note->setFocus();
}

void KNotesApp::createNote( KCal::Journal *journal )
{
  if( journal->uid() == m_noteUidModify)
  {
      KNote *note = m_notes.value( m_noteUidModify );
      if ( note )
          note->changeJournal(journal);
      return;
  }

  m_noteUidModify = journal->uid();
  KNote *newNote = new KNote( m_noteGUI, journal, 0 );
  m_notes.insert( newNote->noteId(), newNote );

  connect( newNote, SIGNAL(sigRequestNewNote()),
           SLOT(newNote()) );
  connect( newNote, SIGNAL(sigShowNextNote()),
           SLOT(slotWalkThroughNotes()) ) ;
  connect( newNote, SIGNAL(sigKillNote(KCal::Journal*)),
           SLOT(slotNoteKilled(KCal::Journal*)) );
  connect( newNote, SIGNAL(sigNameChanged(QString)),
           SLOT(updateNoteActions()) );
  connect( newNote, SIGNAL(sigDataChanged(QString)),
           SLOT(saveNotes(QString)) );
  connect( newNote, SIGNAL(sigColorChanged()),
           SLOT(updateNoteActions()) );
  connect( newNote, SIGNAL(sigFindFinished()),
           SLOT(slotFindNext()) );

  // don't call this during startup for each and every loaded note
  if ( m_alarm ) {
    updateNoteActions();
  }
  //TODO
#if 0
  if (m_tray->isVisible()) {
    // we already booted, so this is a new note
    // sucks, semantically speaking
    newNote->slotRename();
  }
#endif
}

void KNotesApp::killNote( KCal::Journal *journal )
{
  if(m_noteUidModify == journal->uid())
  {
          return;
  }
  // this kills the KNote object
  KNote *note = m_notes.take( journal->uid() );
  if ( note )
  {
    delete note;
    updateNoteActions();
  }
}

void KNotesApp::acceptConnection()
{
  // Accept the connection and make KNotesNetworkReceiver do the job
  QTcpSocket *s = m_listener->nextPendingConnection();

  if ( s ) {
    KNotesNetworkReceiver *recv = new KNotesNetworkReceiver( s );
    connect( recv,
             SIGNAL(sigNoteReceived(QString,QString)),
             SLOT(newNote(QString,QString)) );
  }
}

void KNotesApp::saveNotes( const QString & uid )
{
  m_noteUidModify = uid;
  saveNotes();
}

void KNotesApp::saveNotes()
{
  KNotesGlobalConfig::self()->writeConfig();
  m_manager->save();
}

void KNotesApp::saveConfigs()
{
  foreach ( KNote *note, m_notes ) {
    note->saveConfig();
  }
}

void KNotesApp::updateNoteActions()
{
  unplugActionList( "notes" );
  m_noteActions.clear();

  foreach ( KNote *note, m_notes ) {
    // what does this actually mean? ~gamaral
#ifdef __GNUC__
#warning utf8: use QString
#endif
    KAction *action = new KAction( note->name().replace( "&", "&&" ), this );
                action->setObjectName( note->noteId() );
    connect( action, SIGNAL(triggered(bool)), SLOT(slotShowNote()) );
    KIconEffect effect;
    QPixmap icon =
      effect.apply( qApp->windowIcon().pixmap( IconSize( KIconLoader::Small ),
                                               IconSize( KIconLoader::Small ) ),
                    KIconEffect::Colorize,
                    1,
                    note->palette().color( note->backgroundRole() ),
                    false );

    action->setIcon( icon );
    m_noteActions.append( action );
  }

  if ( m_noteActions.isEmpty() ) {
      actionCollection()->action( "hide_all_notes" )->setEnabled( false );
      actionCollection()->action( "show_all_notes" )->setEnabled( false );
      m_findAction->setEnabled( false );
    KAction *action = new KAction( i18n( "No Notes" ), this );
    m_noteActions.append( action );
  }
  else
  {
      qSort( m_noteActions.begin(), m_noteActions.end(), qActionLessThan );
      actionCollection()->action( "hide_all_notes" )->setEnabled( true );
      actionCollection()->action( "show_all_notes" )->setEnabled( true );
      m_findAction->setEnabled( true );
  }
  plugActionList( "notes", m_noteActions );
}

void KNotesApp::updateNetworkListener()
{
    delete m_listener;
    m_listener=0;
    delete m_publisher;
    m_publisher=0;

    if ( KNotesGlobalConfig::receiveNotes() ) {
        // create the socket and start listening for connections
        m_listener=KSocketFactory::listen( "knotes" , QHostAddress::Any,
                                           KNotesGlobalConfig::port() );
        connect( m_listener, SIGNAL(newConnection()),
                 SLOT(acceptConnection()) );
        m_publisher=new DNSSD::PublicService(KNotesGlobalConfig::senderID(), "_knotes._tcp", KNotesGlobalConfig::port());
        m_publisher->publishAsync();
    }
}

void KNotesApp::updateStyle()
{
#ifdef __GNUC__
#warning FIXME!
#endif
  //    KNote::setStyle( KNotesGlobalConfig::style() );

  foreach ( KNote *note, m_notes ) {
    QApplication::postEvent( note, new QEvent( QEvent::LayoutRequest ) );
  }
}

#include "knotesapp.moc"
