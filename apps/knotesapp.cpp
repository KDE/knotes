/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2013, The KNotes Developers

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

#include "configdialog/knoteconfigdialog.h"

#include "dialog/knoteselectednotesdialog.h"
#include "utils/knoteutils.h"

#include "noteshared/akonadi/notesakonaditreemodel.h"
#include "noteshared/akonadi/noteschangerecorder.h"
#include "noteshared/attributes/notelockattribute.h"
#include "noteshared/attributes/notedisplayattribute.h"
#include "noteshared/attributes/notealarmattribute.h"
#include "noteshared/attributes/showfoldernotesattribute.h"
#include "noteshared/resources/localresourcecreator.h"
#include "noteshared/job/createnewnotejob.h"

#include "apps/knotesakonaditray.h"
#include "dialog/selectednotefolderdialog.h"

#include "notesharedglobalconfig.h"
#include "notes/knote.h"
#include "knotesadaptor.h"
#include "knotesapp.h"
#include "print/knoteprinter.h"
#include "print/knoteprintobject.h"
#include "knotesglobalconfig.h"
#include "dialog/knoteskeydialog.h"
#include "print/knoteprintselectednotesdialog.h"
#include "finddialog/knotefinddialog.h"

#include <KMime/KMimeMessage>
#include "akonadi_next/note.h"
#include <Akonadi/ItemCreateJob>
#include <Akonadi/ItemDeleteJob>
#include <Akonadi/EntityDisplayAttribute>

#include <akonadi/control.h>
#include <Akonadi/ChangeRecorder>
#include <Akonadi/Collection>
#include <Akonadi/EntityTreeModel>
#include <Akonadi/Session>
#include <KMime/KMimeMessage>
#include <KActionCollection>

#include <KMessageBox>
#include <kaction.h>
#include <kdebug.h>
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

#include <kiconloader.h>

#include <QPixmap>
#include <QClipboard>
#include <QDBusConnection>
#include <QTcpServer>

#include <dnssd/publicservice.h>

static bool qActionLessThan( const QAction *a1, const QAction *a2 )
{
    return ( a1->text() < a2->text() );
}


KNotesApp::KNotesApp()
    : QWidget(),
      m_publisher( 0 )
{
    Akonadi::Control::widgetNeedsAkonadi(this);

    KNoteUtils::migrateToAkonadi();

    if (KNotesGlobalConfig::self()->autoCreateResourceOnStart()) {
        NoteShared::LocalResourceCreator *creator = new NoteShared::LocalResourceCreator( this );
        creator->createIfMissing();
    }

    new KNotesAdaptor( this );
    QDBusConnection::sessionBus().registerObject( QLatin1String("/KNotes") , this );
    kapp->setQuitOnLastWindowClosed( false );
    // create the GUI...
    KAction *action  = new KAction( KIcon( QLatin1String("document-new") ),
                                    i18n( "New Note" ), this );
    actionCollection()->addAction( QLatin1String("new_note"), action );
    action->setGlobalShortcut( KShortcut( Qt::ALT + Qt::SHIFT + Qt::Key_N ));
    connect( action, SIGNAL(triggered()), SLOT(newNote()) );

    action  = new KAction( KIcon( QLatin1String("edit-paste") ),
                           i18n( "New Note From Clipboard" ), this );
    actionCollection()->addAction( QLatin1String("new_note_clipboard"), action );
    action->setGlobalShortcut( KShortcut( Qt::ALT + Qt::SHIFT + Qt::Key_C ));
    connect( action, SIGNAL(triggered()), SLOT(newNoteFromClipboard()) );

    action  = new KAction( KIcon(QLatin1String( "knotes") ), i18n( "Show All Notes" ), this );
    actionCollection()->addAction( QLatin1String("show_all_notes"), action );
    action->setGlobalShortcut( KShortcut( Qt::ALT + Qt::SHIFT + Qt::Key_S ));
    connect( action, SIGNAL(triggered()), SLOT(showAllNotes()) );

    action  = new KAction( KIcon( QLatin1String("window-close") ),
                           i18n( "Hide All Notes" ), this );
    actionCollection()->addAction( QLatin1String("hide_all_notes"), action );
    action->setGlobalShortcut( KShortcut( Qt::ALT + Qt::SHIFT + Qt::Key_H ));
    connect( action, SIGNAL(triggered()), SLOT(hideAllNotes()) );

    action = new KAction( KIcon( QLatin1String("document-print") ),
                          i18nc( "@action:inmenu", "Print Selected Notes..." ), this );
    actionCollection()->addAction( QLatin1String("print_selected_notes"), action );
    connect( action, SIGNAL(triggered()), SLOT(slotPrintSelectedNotes()) );

    mFindAction = KStandardAction::find( this, SLOT(slotOpenFindDialog()), actionCollection());



    new KHelpMenu( this, KGlobal::mainComponent().aboutData(), false,
                   actionCollection() );

    KStandardAction::preferences( this, SLOT(slotPreferences()),
                                  actionCollection() );
    KStandardAction::keyBindings( this, SLOT(slotConfigureAccels()),
                                  actionCollection() );
    //FIXME: no shortcut removing!?
    KStandardAction::quit( this, SLOT(slotQuit()),
                           actionCollection() )->setShortcut( 0 );
    setXMLFile( componentData().componentName() + QLatin1String("appui.rc") );

    m_guiBuilder = new KXMLGUIBuilder( this );
    m_guiFactory = new KXMLGUIFactory( m_guiBuilder, this );
    m_guiFactory->addClient( this );

    m_contextMenu = static_cast<KMenu *>( m_guiFactory->container(
                                              QLatin1String("knotes_context"),
                                              this ) );
    m_noteMenu = static_cast<KMenu *>( m_guiFactory->container(
                                           QLatin1String("notes_menu"), this ) );

    // get the most recent XML UI file
    QString xmlFileName = componentData().componentName() + QLatin1String("ui.rc");
    QString filter = componentData().componentName() + QLatin1Char('/') + xmlFileName;
    const QStringList fileList =
            componentData().dirs()->findAllResources( "data", filter ) +
            componentData().dirs()->findAllResources( "data", xmlFileName );

    QString doc;
    KXMLGUIClient::findMostRecentXMLFile( fileList, doc );
    m_noteGUI.setContent( doc );
    // set up the alarm reminder - do it after loading the notes because this
    // is used as a check if updateNoteActions has to be called for a new note
    updateNetworkListener();

    Akonadi::Session *session = new Akonadi::Session( "KNotes Session", this );
    mNoteRecorder = new NoteShared::NotesChangeRecorder(this);
    mNoteRecorder->changeRecorder()->setSession(session);
    mTray = new KNotesAkonadiTray(0);
    connect( mTray, SIGNAL(activateRequested(bool,QPoint)), this, SLOT(slotActivateRequested(bool,QPoint)) );
    connect( mTray, SIGNAL(secondaryActivateRequested(QPoint)), this, SLOT(slotSecondaryActivateRequested(QPoint)) );

    mTray->setContextMenu( m_contextMenu );
    mNoteTreeModel = new NoteShared::NotesAkonadiTreeModel(mNoteRecorder->changeRecorder(), this);

    connect( mNoteTreeModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
             SLOT(slotRowInserted(QModelIndex,int,int)));

    connect( mNoteRecorder->changeRecorder(), SIGNAL(itemChanged(Akonadi::Item,QSet<QByteArray>)), SLOT(slotItemChanged(Akonadi::Item,QSet<QByteArray>)));
    connect( mNoteRecorder->changeRecorder(), SIGNAL(itemRemoved(Akonadi::Item)), SLOT(slotItemRemoved(Akonadi::Item)) );
    updateNoteActions();
}

KNotesApp::~KNotesApp()
{
    qDeleteAll( m_noteActions );
    m_noteActions.clear();
    saveNotes();
    delete m_guiBuilder;
    delete mTray;
    qDeleteAll(mNotes);
    mNotes.clear();
    delete m_publisher;
    m_publisher=0;
}

void KNotesApp::slotItemRemoved(const Akonadi::Item &item)
{
    qDebug()<<" note removed"<<item.id();
    if (mNotes.contains(item.id())) {
        delete mNotes.find(item.id()).value();
        mNotes.remove(item.id());
        updateNoteActions();
        updateSystray();
    }
}

void KNotesApp::slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &set)
{
    if (mNotes.contains(item.id())) {
        qDebug()<<" item changed "<<item.id()<<" info "<<set.toList();
        KNote *note = mNotes.value(item.id());
        note->setChangeItem(item, set);
    }
}

void KNotesApp::slotRowInserted(const QModelIndex &parent, int start, int end)
{
    for ( int i = start; i <= end; ++i) {
        if ( mNoteTreeModel->hasIndex( i, 0, parent ) ) {
            const QModelIndex child = mNoteTreeModel->index( i, 0, parent );
            Akonadi::Item item =
                    mNoteTreeModel->data( child, Akonadi::EntityTreeModel::ItemRole ).value<Akonadi::Item>();
            Akonadi::Collection parentCollection = mNoteTreeModel->data( child, Akonadi::EntityTreeModel::ParentCollectionRole).value<Akonadi::Collection>();
            if (parentCollection.hasAttribute<NoteShared::ShowFolderNotesAttribute>()) {
                if ( !item.hasPayload<KMime::Message::Ptr>() )
                    continue;
                KNote *note = new KNote(m_noteGUI,item, 0);
                mNotes.insert(item.id(), note);
                connect( note, SIGNAL(sigShowNextNote()),
                         SLOT(slotWalkThroughNotes()) ) ;
                connect( note, SIGNAL(sigRequestNewNote()),
                         SLOT(newNote()) );
                connect( note, SIGNAL(sigNameChanged(QString)),
                         SLOT(updateNoteActions()) );
                connect( note, SIGNAL(sigColorChanged()),
                         SLOT(updateNoteActions()) );
                connect( note, SIGNAL(sigKillNote(Akonadi::Item::Id)),
                         SLOT(slotNoteKilled(Akonadi::Item::Id)) );
                updateNoteActions();
                updateSystray();
            }
        }
    }
}

void KNotesApp::updateSystray()
{
    if (KNotesGlobalConfig::self()->systemTrayShowNotes()) {
        mTray->updateNumberOfNotes(mNotes.count());
    }
}

void KNotesApp::newNote(const QString &name, const QString &text)
{
    NoteShared::CreateNewNoteJob *job = new NoteShared::CreateNewNoteJob(this, this);
    job->setRichText(KNotesGlobalConfig::self()->richText());
    job->setNote(name, text);
    job->start();
}

void KNotesApp::showNote(const Akonadi::Entity::Id &id ) const
{
    if (mNotes.contains(id)) {
        KNote *note = mNotes.value(id);
        showNote( note );
    } else {
        kWarning( 5500 ) << "hideNote: no note with id:" << id;
    }
}

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


void KNotesApp::hideNote( const Akonadi::Item::Id &id ) const
{
    if (mNotes.contains(id)) {
        KNote *note = mNotes.value(id);
        note->hide();
    } else {
        kWarning( 5500 ) << "hideNote: no note with id:" << id;
    }
}


void KNotesApp::hideAllNotes() const
{
    QHashIterator<Akonadi::Item::Id, KNote*> i(mNotes);
    while (i.hasNext()) {
        i.next();
        i.value()->slotClose();
    }
}

void KNotesApp::showAllNotes() const
{
    QHashIterator<Akonadi::Item::Id, KNote*> i(mNotes);
    while (i.hasNext()) {
        i.next();
        // workaround to BUG 149116
        i.value()->hide();

        i.value()->show();
    }
}


void KNotesApp::newNoteFromClipboard( const QString &name )
{
    const QString &text = KApplication::clipboard()->text();
    newNote( name, text );
}

void KNotesApp::updateNetworkListener()
{
    delete m_publisher;
    m_publisher=0;

    if ( NoteShared::NoteSharedGlobalConfig::receiveNotes() ) {
        // create the socket and start listening for connections
        m_publisher=new DNSSD::PublicService(NoteShared::NoteSharedGlobalConfig::senderID(), QLatin1String("_knotes._tcp"), NoteShared::NoteSharedGlobalConfig::port());
        m_publisher->publishAsync();
    }
}

QString KNotesApp::name( const Akonadi::Item::Id &id ) const
{
    if (mNotes.contains(id)) {
        return mNotes.value(id)->name();
    }
    return QString();
}

QString KNotesApp::text( const Akonadi::Item::Id &id ) const
{
    if (mNotes.contains(id)) {
        return mNotes.value(id)->text();
    }
    return QString();
}

void KNotesApp::setName( const Akonadi::Item::Id &id, const QString &newName )
{
    if (mNotes.contains(id)) {
        mNotes.value(id)->setName(newName);
    } else {
        kWarning( 5500 ) << "setName: no note with id:" << id;
    }
}

void KNotesApp::setText( const Akonadi::Item::Id &id, const QString &newText )
{
    if (mNotes.contains(id)) {
        mNotes.value(id)->setText( newText );
    } else {
        kWarning( 5500 ) << "setText: no note with id:" << id;
    }
}

void KNotesApp::updateNoteActions()
{
    unplugActionList( QLatin1String("notes") );
    m_noteActions.clear();

    QHashIterator<Akonadi::Item::Id, KNote*> i(mNotes);
    while (i.hasNext()) {
        i.next();
        KNote *note = i.value();
        KAction *action = new KAction( note->name().replace( QLatin1String("&"), QLatin1String("&&") ), this );
        action->setObjectName(QString::number(note->noteId()));
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
        actionCollection()->action( QLatin1String("hide_all_notes") )->setEnabled( false );
        actionCollection()->action( QLatin1String("show_all_notes") )->setEnabled( false );
        actionCollection()->action( QLatin1String("print_selected_notes") )->setEnabled( false );
        KAction *action = new KAction( i18n( "No Notes" ), this );
        m_noteActions.append( action );
    } else {
        qSort( m_noteActions.begin(), m_noteActions.end(), qActionLessThan );
        actionCollection()->action( QLatin1String("hide_all_notes") )->setEnabled( true );
        actionCollection()->action( QLatin1String("show_all_notes") )->setEnabled( true );
        actionCollection()->action( QLatin1String("print_selected_notes") )->setEnabled( true );
    }
    plugActionList( QLatin1String("notes"), m_noteActions );
}


void KNotesApp::slotActivateRequested( bool, const QPoint&)
{
    if ( mNotes.size() == 1 ) {
        showNote( mNotes.begin().value() );
    } else {
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
    showNote( sender()->objectName().toLongLong() );
}

void KNotesApp::slotWalkThroughNotes()
{
    QHashIterator<Akonadi::Item::Id, KNote*> i(mNotes);
    while (i.hasNext()) {
        i.next();
        KNote *note = i.value();
        if ( note->hasFocus() ) {
            if ( i.value() != mNotes.end().value() ) {
                showNote( i.value() );
            } else {
                showNote( mNotes.begin().value() );
            }
            break;
        }
    }
}

void KNotesApp::slotPreferences()
{
    // create a new preferences dialog...
    KNoteConfigDialog *dialog = new KNoteConfigDialog( i18n( "Settings" ), this);
    connect( dialog, SIGNAL(configWrote()), this, SLOT(slotConfigUpdated()));
    dialog->show();
}

void KNotesApp::slotConfigUpdated()
{
    updateNetworkListener();
    //Force update if we disable or enable show number in systray
    mTray->updateNumberOfNotes(mNotes.count());
}

void KNotesApp::slotConfigureAccels()
{
    QPointer<KNotesKeyDialog> keys = new KNotesKeyDialog( actionCollection(), this );

    KActionCollection *actionCollection = 0;
    if ( !mNotes.isEmpty() ) {
        actionCollection = mNotes.begin().value()->actionCollection();
        keys->insert( actionCollection );
    }
    if (keys->exec()) {
        keys->save();

        // update GUI doc for new notes
        m_noteGUI.setContent(
                    KXMLGUIFactory::readConfigFile( componentData().componentName() + QLatin1String("ui.rc"),
                                                    componentData() )
                    );


        if ( actionCollection ) {
            QHashIterator<Akonadi::Item::Id, KNote*> i(mNotes);
            while (i.hasNext()) {
                i.next();
                foreach ( QAction *action, actionCollection->actions() ) {
                    QAction *toChange = i.value()->actionCollection()->action( action->objectName() );
                    if ( toChange ) {
                        toChange->setShortcuts( action->shortcuts() );
                    }
                }

            }
        }
    }
    delete keys;
}

void KNotesApp::slotNoteKilled( Akonadi::Item::Id id)
{
    Akonadi::ItemDeleteJob *deleteJob = new Akonadi::ItemDeleteJob(Akonadi::Item(id), this);
    connect( deleteJob, SIGNAL(result(KJob*)), SLOT(slotNoteDeleteFinished(KJob*)) );
}

void KNotesApp::slotNoteDeleteFinished(KJob* job)
{
    if (job->error()) {
        kWarning() << job->errorString();
        return;
    }
}

void KNotesApp::slotPrintSelectedNotes()
{
    QPointer<KNotePrintSelectedNotesDialog> dlg = new KNotePrintSelectedNotesDialog(this);
    dlg->setNotes(mNotes);
    if (dlg->exec()) {
        const QList<KNotePrintObject *> lst = dlg->selectedNotes();
        if (!lst.isEmpty()) {
            const QString selectedTheme = dlg->selectedTheme();
            KNotePrinter printer;
            printer.printNotes( lst, selectedTheme, dlg->preview() );
            qDeleteAll(lst);
        }
    }
    delete dlg;
}

void KNotesApp::saveNotes(bool force)
{
    KNotesGlobalConfig::self()->writeConfig();
    QHashIterator<Akonadi::Item::Id, KNote*> i(mNotes);
    while (i.hasNext()) {
        i.next();
        i.value()->saveNote(force);
    }
}

void KNotesApp::slotQuit()
{
    saveNotes(true);
    kapp->quit();
}

bool KNotesApp::commitData( QSessionManager & )
{
    saveNotes();
    return true;
}

void KNotesApp::slotOpenFindDialog()
{
    QPointer<KNoteFindDialog> dlg = new KNoteFindDialog(this);
    dlg->exec();
    delete dlg;
}
