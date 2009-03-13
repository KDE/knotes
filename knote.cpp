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

#include <qlabel.h>
#include <qdrawutil.h>
#include <qsize.h>
#include <qsizegrip.h>
#include <qbitmap.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>
#include <qobjectlist.h>
#include <qfile.h>
#include <qcheckbox.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kcombobox.h>
#include <ktoolbar.h>
#include <kpopupmenu.h>
#include <kxmlguibuilder.h>
#include <kxmlguifactory.h>
#include <kcolordrag.h>
#include <kiconeffect.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kfind.h>
#include <kprocess.h>
#include <kinputdialog.h>
#include <kmdcodec.h>
#include <kglobalsettings.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>

#include <libkcal/journal.h>

#include "knote.h"
#include "knotebutton.h"
#include "knoteedit.h"
#include "knoteconfig.h"
#include "knotesglobalconfig.h"
#include "knoteconfigdlg.h"
#include "knotealarmdlg.h"
#include "knotehostdlg.h"
#include "knotesnetsend.h"
#include "knoteprinter.h"
#include "version.h"

#include "pushpin.xpm"

#include <kwin.h>
#include <netwm.h>

#include <fixx11h.h>

using namespace KCal;

extern Time qt_x_time;

int KNote::s_ppOffset = 0;

KNote::KNote( QDomDocument buildDoc, Journal *j, QWidget *parent, const char *name )
  : QFrame( parent, name, WStyle_Customize | WStyle_NoBorder | WDestructiveClose ),
    m_label( 0 ), m_pushpin( 0 ), m_fold( 0 ), m_button( 0 ), m_tool( 0 ), m_editor( 0 ),
    m_config( 0 ), m_journal( j ), m_find( 0 ),
    m_kwinConf( KSharedConfig::openConfig( "kwinrc", true ) ),
    m_busy( 0 ), m_deleteWhenIdle( false ), m_blockEmitDataChanged( false )
{
    setAcceptDrops( true );
    actionCollection()->setWidget( this );

    setDOMDocument( buildDoc );

    // just set the name of the file to save the actions to, do NOT reparse it
    setXMLFile( instance()->instanceName() + "ui.rc", false, false );

    // if there is no title yet, use the start date if valid
    // (KOrganizer's journals don't have titles but a valid start date)
    if ( m_journal->summary().isNull() && m_journal->dtStart().isValid() )
    {
        QString s = KGlobal::locale()->formatDateTime( m_journal->dtStart() );
        m_journal->setSummary( s );
    }

    // create the menu items for the note - not the editor...
    // rename, mail, print, save as, insert date, alarm, close, delete, new note
    new KAction( i18n("New"), "filenew", 0,
        this, SIGNAL(sigRequestNewNote()), actionCollection(), "new_note" );
    new KAction( i18n("Rename..."), "text", 0,
        this, SLOT(slotRename()), actionCollection(), "rename_note" );
    m_readOnly = new KToggleAction( i18n("Lock"), "lock" , 0,
        this, SLOT(slotUpdateReadOnly()), actionCollection(), "lock_note" );
    m_readOnly->setCheckedState( KGuiItem( i18n("Unlock"), "unlock" ) );
    new KAction( i18n("Hide"), "fileclose" , Key_Escape,
        this, SLOT(slotClose()), actionCollection(), "hide_note" );
    new KAction( i18n("Delete"), "knotes_delete", 0,
        this, SLOT(slotKill()), actionCollection(), "delete_note" );

    new KAction( i18n("Insert Date"), "knotes_date", 0 ,
        this, SLOT(slotInsDate()), actionCollection(), "insert_date" );
    new KAction( i18n("Set Alarm..."), "knotes_alarm", 0 ,
        this, SLOT(slotSetAlarm()), actionCollection(), "set_alarm" );

    new KAction( i18n("Send..."), "network", 0,
        this, SLOT(slotSend()), actionCollection(), "send_note" );
    new KAction( i18n("Mail..."), "mail_send", 0,
        this, SLOT(slotMail()), actionCollection(), "mail_note" );
    new KAction( i18n("Save As..."), "filesaveas", 0,
        this, SLOT(slotSaveAs()), actionCollection(), "save_note" );
    KStdAction::print( this, SLOT(slotPrint()), actionCollection(), "print_note" );
    new KAction( i18n("Preferences..."), "configure", 0,
        this, SLOT(slotPreferences()), actionCollection(), "configure_note" );

    m_keepAbove = new KToggleAction( i18n("Keep Above Others"), "up", 0,
        this, SLOT(slotUpdateKeepAboveBelow()), actionCollection(), "keep_above" );
    m_keepAbove->setExclusiveGroup( "keepAB" );

    m_keepBelow = new KToggleAction( i18n("Keep Below Others"), "down", 0,
        this, SLOT(slotUpdateKeepAboveBelow()), actionCollection(), "keep_below" );
    m_keepBelow->setExclusiveGroup( "keepAB" );

    m_toDesktop = new KListAction( i18n("To Desktop"), 0,
        this, SLOT(slotPopupActionToDesktop(int)), actionCollection(), "to_desktop" );
    connect( m_toDesktop->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(slotUpdateDesktopActions()) );

    // invisible action to walk through the notes to make this configurable
    new KAction( i18n("Walk Through Notes"), 0, SHIFT+Key_BackTab,
                 this, SIGNAL(sigShowNextNote()), actionCollection(), "walk_notes" );

    // create the note header, button and label...
    m_label = new QLabel( this );
    m_label->setFrameStyle( NoFrame );
    m_label->setLineWidth( 0 );
    m_label->installEventFilter( this );  // receive events (for dragging & action menu)
    setName( m_journal->summary() );      // don't worry, no signals are connected at this stage yet

    m_button = new KNoteButton( "knotes_close", this );
    connect( m_button, SIGNAL(clicked()), this, SLOT(slotClose()) );

    // create the note editor
    m_editor = new KNoteEdit( actionCollection(), this );
    m_editor->installEventFilter( this ); // receive events (for modified)
    m_editor->viewport()->installEventFilter( this );
    connect( m_editor, SIGNAL(contentsMoving( int, int )), this, SLOT(slotUpdateViewport( int, int )));

    KXMLGUIBuilder builder( this );
    KXMLGUIFactory factory( &builder, this );
    factory.addClient( this );

    m_menu = dynamic_cast<KPopupMenu*>(factory.container( "note_context", this ));
    m_edit_menu = dynamic_cast<KPopupMenu*>(factory.container( "note_edit", this ));
    m_tool = dynamic_cast<KToolBar*>(factory.container( "note_tool", this ));

    if ( m_tool ) {
      m_tool->setIconSize( 10 );
      m_tool->setFixedHeight( 16 );
      m_tool->setIconText( KToolBar::IconOnly );

      // if there was just a way of making KComboBox adhere the toolbar height...
      QObjectList *list = m_tool->queryList( "KComboBox" );
      QObjectListIt it( *list );
      while ( it.current() != 0 )
      {
          KComboBox *combo = (KComboBox *)it.current();
          QFont font = combo->font();
          font.setPointSize( 7 );
          combo->setFont( font );
          combo->setFixedHeight( 14 );
          ++it;
      }
      delete list;

      m_tool->hide();
    }

    setFocusProxy( m_editor );

    // create the resize handle
    m_editor->setCornerWidget( new QSizeGrip( this ) );
    uint width = m_editor->cornerWidget()->width();
    uint height = m_editor->cornerWidget()->height();
    QBitmap mask;
    mask.resize( width, height );
    mask.fill( color0 );
    QPointArray array;
    array.setPoints( 3, 0, height, width, height, width, 0 );
    QPainter p;
    p.begin( &mask );
    p.setBrush( color1 );
    p.drawPolygon( array );
    p.end();
    m_editor->cornerWidget()->setMask( mask );
    m_editor->cornerWidget()->setBackgroundMode( PaletteBase );

    // the config file location
    QString configFile = KGlobal::dirs()->saveLocation( "appdata", "notes/" );
    configFile += m_journal->uid();

    // no config file yet? -> use the default display config if available
    // we want to write to configFile, so use "false"
    bool newNote = !KIO::NetAccess::exists( KURL::fromPathOrURL( configFile ), false, 0 );

    m_config = new KNoteConfig( KSharedConfig::openConfig( configFile, false, false ) );
    m_config->readConfig();
    m_config->setVersion( KNOTES_VERSION );

    if ( newNote )
    {
        // until kdelibs provides copying of KConfigSkeletons (KDE 3.4)
        KNotesGlobalConfig *globalConfig = KNotesGlobalConfig::self();
        m_config->setBgColor( globalConfig->bgColor() );
        m_config->setFgColor( globalConfig->fgColor() );
        m_config->setWidth( globalConfig->width() );
        m_config->setHeight( globalConfig->height() );

        m_config->setFont( globalConfig->font() );
        m_config->setTitleFont( globalConfig->titleFont() );
        m_config->setAutoIndent( globalConfig->autoIndent() );
        m_config->setRichText( globalConfig->richText() );
        m_config->setTabSize( globalConfig->tabSize() );
        m_config->setReadOnly( globalConfig->readOnly() );

        m_config->setDesktop( globalConfig->desktop() );
        m_config->setHideNote( globalConfig->hideNote() );
        m_config->setPosition( globalConfig->position() );
        m_config->setShowInTaskbar( globalConfig->showInTaskbar() );
        m_config->setKeepAbove( globalConfig->keepAbove() );
        m_config->setKeepBelow( globalConfig->keepBelow() );

        m_config->writeConfig();
    }

    // set up the look&feel of the note
    setMinimumSize( 20, 20 );
    setLineWidth( 1 );
    setMargin( 0 );

    m_editor->setMargin( 0 );
    m_editor->setFrameStyle( NoFrame );
    m_editor->setBackgroundOrigin( WindowOrigin );

    // can be done here since this doesn't pick up changes while KNotes is running anyway
    bool closeLeft = false;
    m_kwinConf->setGroup( "Style" );
    if ( m_kwinConf->readBoolEntry( "CustomButtonPositions" ) )
        closeLeft = m_kwinConf->readEntry( "ButtonsOnLeft" ).find( 'X' ) > -1;

    QPixmap pushpin_pix;
    if ( closeLeft )
        pushpin_pix = QPixmap( QPixmap( pushpin_xpm ).convertToImage().mirror( true, false ) );
    else
        pushpin_pix = QPixmap( pushpin_xpm );

    // the pushpin label at the top left or right corner
    m_pushpin = new QLabel( this );
    m_pushpin->setScaledContents( true );
    m_pushpin->setBackgroundMode( NoBackground );
    m_pushpin->setPixmap( pushpin_pix );
    m_pushpin->resize( pushpin_pix.size() );

    // fold label at bottom right corner
    m_fold = new QLabel( this );
    m_fold->setScaledContents( true );
    m_fold->setBackgroundMode( NoBackground );

    // load the display configuration of the note
    width = m_config->width();
    height = m_config->height();
    resize( width, height );

    // let KWin do the placement if the position is illegal--at least 10 pixels
    // of a note need to be visible
    const QPoint& position = m_config->position();
    QRect desk = kapp->desktop()->rect();
    desk.addCoords( 10, 10, -10, -10 );
    if ( desk.intersects( QRect( position, QSize( width, height ) ) ) )
        move( position );           // do before calling show() to avoid flicker

    // config items in the journal have priority
    QString property = m_journal->customProperty( "KNotes", "FgColor" );
    if ( !property.isNull() )
        m_config->setFgColor( QColor( property ) );
    else
        m_journal->setCustomProperty( "KNotes", "FgColor", m_config->fgColor().name() );

    property = m_journal->customProperty( "KNotes", "BgColor" );
    if ( !property.isNull() )
        m_config->setBgColor( QColor( property ) );
    else
        m_journal->setCustomProperty( "KNotes", "BgColor", m_config->bgColor().name() );

    property = m_journal->customProperty( "KNotes", "RichText" );
    if ( !property.isNull() )
        m_config->setRichText( property == "true" ? true : false );
    else
        m_journal->setCustomProperty( "KNotes", "RichText", m_config->richText() ? "true" : "false" );

    // read configuration settings...
    slotApplyConfig();

    // create the mask for the fold---to be done after slotApplyConfig(),
    // which calls createFold()
    m_fold->setMask( QRegion( m_fold->pixmap()->createHeuristicMask() ) );

    // if this is a new note put on current desktop - we can't use defaults
    // in KConfig XT since only _changes_ will be stored in the config file
    int desktop = m_config->desktop();
    if ( desktop < 0 && desktop != NETWinInfo::OnAllDesktops )
        desktop = KWin::currentDesktop();

    // show the note if desired
    if ( desktop != 0 && !m_config->hideNote() )
    {
        // to avoid flicker, call this before show()
        toDesktop( desktop );
        show();

        // because KWin forgets about that for hidden windows
        if ( desktop == NETWinInfo::OnAllDesktops )
            toDesktop( desktop );
    }

    m_editor->setText( m_journal->description() );
    m_editor->setModified( false );

    m_readOnly->setChecked( m_config->readOnly() );
    slotUpdateReadOnly();

    if ( m_config->keepAbove() )
        m_keepAbove->setChecked( true );
    else if ( m_config->keepBelow() )
        m_keepBelow->setChecked( true );
    else
    {
        m_keepAbove->setChecked( false );
        m_keepBelow->setChecked( false );
    }
    slotUpdateKeepAboveBelow();

    // HACK: update the icon color - again after showing the note, to make kicker aware of the new colors
    KIconEffect effect;
    QPixmap icon = effect.apply( kapp->icon(), KIconEffect::Colorize, 1, m_config->bgColor(), false );
    QPixmap miniIcon = effect.apply( kapp->miniIcon(), KIconEffect::Colorize, 1, m_config->bgColor(), false );
    KWin::setIcons( winId(), icon, miniIcon );
}

KNote::~KNote()
{
    delete m_config;
}


// -------------------- public slots -------------------- //

void KNote::slotKill( bool force )
{
    if ( !force &&
         KMessageBox::warningContinueCancel( this,
             i18n("<qt>Do you really want to delete note <b>%1</b>?</qt>").arg( m_label->text() ),
             i18n("Confirm Delete"), KGuiItem( i18n("&Delete"), "editdelete" ),
             "ConfirmDeleteNote"
         )
         != KMessageBox::Continue )
    {
        return;
    }

    // delete the configuration first, then the corresponding file
    delete m_config;
    m_config = 0;

    QString configFile = KGlobal::dirs()->saveLocation( "appdata", "notes/" );
    configFile += m_journal->uid();

    if ( !KIO::NetAccess::del( KURL::fromPathOrURL( configFile ), this ) )
        kdError(5500) << "Can't remove the note config: " << configFile << endl;

    emit sigKillNote( m_journal );
}


// -------------------- public member functions -------------------- //

void KNote::saveData()
{
    m_journal->setSummary( m_label->text() );
    m_journal->setDescription( m_editor->text() );
    m_journal->setCustomProperty( "KNotes", "FgColor", m_config->fgColor().name() );
    m_journal->setCustomProperty( "KNotes", "BgColor", m_config->bgColor().name() );
    m_journal->setCustomProperty( "KNotes", "RichText", m_config->richText() ? "true" : "false" );

    emit sigDataChanged();
    m_editor->setModified( false );
}

void KNote::saveConfig() const
{
    m_config->setWidth( width() );
    if ( m_tool )
      m_config->setHeight( height() - (m_tool->isHidden() ? 0 : m_tool->height()) );
    else
      m_config->setHeight( 0 );
    m_config->setPosition( pos() );

    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop );
    if ( wm_client.desktop() == NETWinInfo::OnAllDesktops || wm_client.desktop() > 0 )
        m_config->setDesktop( wm_client.desktop() );

    // actually store the config on disk
    m_config->writeConfig();
}

QString KNote::noteId() const
{
    return m_journal->uid();
}

QString KNote::name() const
{
    return m_label->text();
}

QString KNote::text() const
{
    return m_editor->text();
}

QString KNote::plainText() const
{
    if ( m_editor->textFormat() == RichText )
    {
        QTextEdit conv;
        conv.setTextFormat( RichText );
        conv.setText( m_editor->text() );
        conv.setTextFormat( PlainText );
        return conv.text();
    }
    else
        return m_editor->text();
}

void KNote::setName( const QString& name )
{
    m_label->setText( name );
    updateLabelAlignment();

    if ( m_editor )    // not called from CTOR?
        saveData();

    // set the window's name for the taskbar entry to be more helpful (#58338)
    NETWinInfo note_win( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop );
    note_win.setName( name.utf8() );

    emit sigNameChanged();
}

void KNote::setText( const QString& text )
{
    m_editor->setText( text );
    saveData();
}

QColor KNote::fgColor() const
{
    return m_config->fgColor();
}

QColor KNote::bgColor() const
{
    return m_config->bgColor();
}

void KNote::setColor( const QColor& fg, const QColor& bg )
{
    m_journal->setCustomProperty( "KNotes", "FgColor", fg.name() );
    m_journal->setCustomProperty( "KNotes", "BgColor", bg.name() );
    m_config->setFgColor( fg );
    m_config->setBgColor( bg );

    m_journal->updated();  // because setCustomProperty() doesn't call it!!
    emit sigDataChanged();
    m_config->writeConfig();

    QPalette newpalette = palette();
    newpalette.setColor( QColorGroup::Background, bg );
    newpalette.setColor( QColorGroup::Foreground, fg );
    newpalette.setColor( QColorGroup::Base,       bg ); // text background
    newpalette.setColor( QColorGroup::Text,       fg ); // text color
    newpalette.setColor( QColorGroup::Button,     bg );
    newpalette.setColor( QColorGroup::ButtonText, fg );

//    newpalette.setColor( QColorGroup::Highlight,  bg );
//    newpalette.setColor( QColorGroup::HighlightedText, fg );

    // the shadow
    newpalette.setColor( QColorGroup::Midlight, bg.light(150) );
    newpalette.setColor( QColorGroup::Shadow, bg.dark(116) );
    newpalette.setColor( QColorGroup::Light, bg.light(180) );
    if ( s_ppOffset )
        newpalette.setColor( QColorGroup::Dark, bg.dark(200) );
    else
        newpalette.setColor( QColorGroup::Dark, bg.dark(108) );
    setPalette( newpalette );

    // set the text color
    m_editor->setTextColor( fg );

    // set the background color or gradient
    updateBackground();

    // set darker value for the hide button...
    QPalette darker = palette();
    darker.setColor( QColorGroup::Button, bg.dark(116) );
    m_button->setPalette( darker );

    // update the icon color
    KIconEffect effect;
    QPixmap icon = effect.apply( kapp->icon(), KIconEffect::Colorize, 1, bg, false );
    QPixmap miniIcon = effect.apply( kapp->miniIcon(), KIconEffect::Colorize, 1, bg, false );
    KWin::setIcons( winId(), icon, miniIcon );

    // set the color for the selection used to highlight the find stuff
    QColor sel = palette().color( QPalette::Active, QColorGroup::Base ).dark();
    if ( sel == Qt::black )
        sel = palette().color( QPalette::Active, QColorGroup::Base ).light();

    m_editor->setSelectionAttributes( 1, sel, true );

    // update the color of the fold
    createFold();

    // update the color of the title
    updateFocus();
    emit sigColorChanged();
}

void KNote::find( const QString& pattern, long options )
{
    delete m_find;
    m_find = new KFind( pattern, options, this );

    connect( m_find, SIGNAL(highlight( const QString &, int, int )),
             this, SLOT(slotHighlight( const QString &, int, int )) );
    connect( m_find, SIGNAL(findNext()), this, SLOT(slotFindNext()) );

    m_find->setData( plainText() );
    slotFindNext();
}

void KNote::slotFindNext()
{
    // TODO: honor FindBackwards
    // TODO: dialogClosed() -> delete m_find

    // Let KFind inspect the text fragment, and display a dialog if a match is found
    KFind::Result res = m_find->find();

    if ( res == KFind::NoMatch ) // i.e. at end-pos
    {
        m_editor->removeSelection( 1 );
        emit sigFindFinished();
        delete m_find;
        m_find = 0;
    }
    else
    {
        show();
        KWin::setCurrentDesktop( KWin::windowInfo( winId() ).desktop() );
    }
}

void KNote::slotHighlight( const QString& str, int idx, int len )
{
    int paraFrom = 0, idxFrom = 0, p = 0;
    for ( ; p < idx; ++p )
        if ( str[p] == '\n' )
        {
            ++paraFrom;
            idxFrom = 0;
        }
        else
            ++idxFrom;

    int paraTo = paraFrom, idxTo = idxFrom;

    for ( ; p < idx + len; ++p )
    {
        if ( str[p] == '\n' )
        {
            ++paraTo;
            idxTo = 0;
        }
        else
            ++idxTo;
    }

    m_editor->setSelection( paraFrom, idxFrom, paraTo, idxTo, 1 );
}

bool KNote::isModified() const
{
    return m_editor->isModified();
}

// FIXME KDE 4.0: remove sync(), isNew() and isModified()
void KNote::sync( const QString& app )
{
    QByteArray sep( 1 );
    sep[0] = '\0';

    KMD5 hash;
    QCString result;

    hash.update( m_label->text().utf8() );
    hash.update( sep );
    hash.update( m_editor->text().utf8() );
    hash.hexDigest( result );

    // hacky... not possible with KConfig XT
    KConfig *config = m_config->config();
    config->setGroup( "Synchronisation" );
    config->writeEntry( app, result.data() );
}

bool KNote::isNew( const QString& app ) const
{
    KConfig *config = m_config->config();
    config->setGroup( "Synchronisation" );
    QString hash = config->readEntry( app );
    return hash.isEmpty();
}

bool KNote::isModified( const QString& app ) const
{
    QByteArray sep( 1 );
    sep[0] = '\0';

    KMD5 hash;
    hash.update( m_label->text().utf8() );
    hash.update( sep );
    hash.update( m_editor->text().utf8() );
    hash.hexDigest();

    KConfig *config = m_config->config();
    config->setGroup( "Synchronisation" );
    QString orig = config->readEntry( app );

    if ( hash.verify( orig.utf8() ) )   // returns false on error!
        return false;
    else
        return true;
}

void KNote::setStyle( int style )
{
    if ( style == KNotesGlobalConfig::EnumStyle::Plain )
        s_ppOffset = 0;
    else
        s_ppOffset = 12;
}


// ------------------ private slots (menu actions) ------------------ //

void KNote::slotRename()
{
    m_blockEmitDataChanged = true;
    // pop up dialog to get the new name
    bool ok;
    aboutToEnterEventLoop();
    QString oldName = m_label->text();
    QString newName = KInputDialog::getText( QString::null,
        i18n("Please enter the new name:"), m_label->text(), &ok, this );
    eventLoopLeft();
    m_blockEmitDataChanged = false;
    if ( !ok || ( oldName == newName) ) // handle cancel
        return;

    setName( newName );
}

void KNote::slotUpdateReadOnly()
{
    const bool readOnly = m_readOnly->isChecked();

    m_editor->setReadOnly( readOnly );
    m_config->setReadOnly( readOnly );

    // Enable/disable actions accordingly
    actionCollection()->action( "configure_note" )->setEnabled( !readOnly );
    actionCollection()->action( "insert_date" )->setEnabled( !readOnly );
    actionCollection()->action( "delete_note" )->setEnabled( !readOnly );

    actionCollection()->action( "edit_undo" )->setEnabled( !readOnly && m_editor->isUndoAvailable() );
    actionCollection()->action( "edit_redo" )->setEnabled( !readOnly && m_editor->isRedoAvailable() );
    actionCollection()->action( "edit_cut" )->setEnabled( !readOnly && m_editor->hasSelectedText() );
    actionCollection()->action( "edit_paste" )->setEnabled( !readOnly );
    actionCollection()->action( "edit_clear" )->setEnabled( !readOnly );
    actionCollection()->action( "rename_note" )->setEnabled( !readOnly );

    actionCollection()->action( "format_bold" )->setEnabled( !readOnly );
    actionCollection()->action( "format_italic" )->setEnabled( !readOnly );
    actionCollection()->action( "format_underline" )->setEnabled( !readOnly );
    actionCollection()->action( "format_strikeout" )->setEnabled( !readOnly );
    actionCollection()->action( "format_alignleft" )->setEnabled( !readOnly );
    actionCollection()->action( "format_aligncenter" )->setEnabled( !readOnly );
    actionCollection()->action( "format_alignright" )->setEnabled( !readOnly );
    actionCollection()->action( "format_alignblock" )->setEnabled( !readOnly );
    actionCollection()->action( "format_list" )->setEnabled( !readOnly );
    actionCollection()->action( "format_super" )->setEnabled( !readOnly );
    actionCollection()->action( "format_sub" )->setEnabled( !readOnly );
    actionCollection()->action( "format_size" )->setEnabled( !readOnly );
    actionCollection()->action( "format_color" )->setEnabled( !readOnly );

    updateFocus();
}

void KNote::slotClose()
{
    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop );
    if ( wm_client.desktop() == NETWinInfo::OnAllDesktops || wm_client.desktop() > 0 )
        m_config->setDesktop( wm_client.desktop() );

    m_editor->clearFocus();
    m_config->setHideNote( true );
    m_config->setPosition( pos() );

    // just hide the note so it's still available from the dock window
    hide();
}

void KNote::slotInsDate()
{
    m_editor->insert( KGlobal::locale()->formatDateTime(QDateTime::currentDateTime()) );
}

void KNote::slotSetAlarm()
{
    m_blockEmitDataChanged = true;
    KNoteAlarmDlg dlg( name(), this );
    dlg.setIncidence( m_journal );

    aboutToEnterEventLoop();
    if ( dlg.exec() == QDialog::Accepted )
        emit sigDataChanged();
    eventLoopLeft();
    m_blockEmitDataChanged = false;
}

void KNote::slotPreferences()
{
    // reuse if possible
    if ( KNoteConfigDlg::showDialog( noteId().utf8() ) )
        return;

    // create a new preferences dialog...
    KNoteConfigDlg *dialog = new KNoteConfigDlg( m_config, name(), this, noteId().utf8() );
    connect( dialog, SIGNAL(settingsChanged()), this, SLOT(slotApplyConfig()) );
    connect( this, SIGNAL(sigNameChanged()), dialog, SLOT(slotUpdateCaption()) );
    dialog->show();
}

void KNote::slotSend()
{
    m_blockEmitDataChanged = true;
    // pop up dialog to get the IP
    KNoteHostDlg hostDlg( i18n("Send \"%1\"").arg( name() ), this );
    aboutToEnterEventLoop();
    bool ok = (hostDlg.exec() == QDialog::Accepted);
    eventLoopLeft();
    m_blockEmitDataChanged = false;
    if ( !ok ) // handle cancel
        return;
    QString host = hostDlg.host();

    if ( host.isEmpty() )
    {
        KMessageBox::sorry( this, i18n("The host cannot be empty.") );
        return;
    }

    // Send the note
    KNotesNetworkSender *sender = new KNotesNetworkSender( host, KNotesGlobalConfig::port() );
    sender->setSenderId( KNotesGlobalConfig::senderID() );
    sender->setNote( name(), text() );
    sender->connect();
}

void KNote::slotMail()
{
    // get the mail action command
    const QStringList cmd_list = QStringList::split( QChar(' '), KNotesGlobalConfig::mailAction() );

    KProcess mail;
    for ( QStringList::ConstIterator it = cmd_list.constBegin();
        it != cmd_list.constEnd(); ++it )
    {
        if ( *it == "%f" )
            mail << plainText().local8Bit();  // convert rich text to plain text
        else if ( *it == "%t" )
            mail << m_label->text().local8Bit();
        else
            mail << (*it).local8Bit();
    }

    if ( !mail.start( KProcess::DontCare ) )
        KMessageBox::sorry( this, i18n("Unable to start the mail process.") );
}

void KNote::slotPrint()
{
    saveData();

    QString content;
    if ( m_editor->textFormat() == PlainText )
        content = QStyleSheet::convertFromPlainText( m_editor->text() );
    else
        content = m_editor->text();

    KNotePrinter printer;
    printer.setMimeSourceFactory( m_editor->mimeSourceFactory() );
    printer.setFont( m_config->font() );
    printer.setContext( m_editor->context() );
    printer.setStyleSheet( m_editor->styleSheet() );
    printer.setColorGroup( colorGroup() );
    printer.printNote( QString(), content );
}

void KNote::slotSaveAs()
{
    QCheckBox *convert = 0;

    if ( m_editor->textFormat() == RichText )
    {
        convert = new QCheckBox( 0 );
        convert->setText( i18n("Save note as plain text") );
    }

    KFileDialog dlg( QString::null, QString::null, this, "filedialog", true, convert );
    dlg.setOperationMode( KFileDialog::Saving );
    dlg.setCaption( i18n("Save As") );
    aboutToEnterEventLoop();
    dlg.exec();
    eventLoopLeft();

    QString fileName = dlg.selectedFile();
    if ( fileName.isEmpty() )
        return;

    QFile file( fileName );

    if ( file.exists() &&
         KMessageBox::warningContinueCancel( this, i18n("<qt>A file named <b>%1</b> already exists.<br>"
                           "Are you sure you want to overwrite it?</qt>").arg( QFileInfo(file).fileName() ) )
         != KMessageBox::Continue )
    {
        return;
    }

    if ( file.open( IO_WriteOnly ) )
    {
        QTextStream stream( &file );
        // convert rich text to plain text first
        if ( convert && convert->isChecked() )
            stream << plainText();
        else
            stream << text();
    }
}

void KNote::slotPopupActionToDesktop( int id )
{
    toDesktop( id - 1 ); // compensate for the menu separator, -1 == all desktops
}


// ------------------ private slots (configuration) ------------------ //

void KNote::slotApplyConfig()
{
    if ( m_config->richText() )
        m_editor->setTextFormat( RichText );
    else
        m_editor->setTextFormat( PlainText );

    m_label->setFont( m_config->titleFont() );
    m_editor->setTextFont( m_config->font() );
    m_editor->setTabStop( m_config->tabSize() );
    m_editor->setAutoIndentMode( m_config->autoIndent() );

    // if called as a slot, save the text, we might have changed the
    // text format - otherwise the journal will not be updated
    if ( sender() )
        saveData();

    setColor( m_config->fgColor(), m_config->bgColor() );

    updateLabelAlignment();
    slotUpdateShowInTaskbar();
}

void KNote::slotUpdateKeepAboveBelow()
{
    KWin::WindowInfo info( KWin::windowInfo( winId() ) );

    if ( m_keepAbove->isChecked() )
    {
        m_config->setKeepAbove( true );
        m_config->setKeepBelow( false );
        KWin::setState( winId(), info.state() | NET::KeepAbove );
    }
    else if ( m_keepBelow->isChecked() )
    {
        m_config->setKeepAbove( false );
        m_config->setKeepBelow( true );
        KWin::setState( winId(), info.state() | NET::KeepBelow );
    }
    else
    {
        m_config->setKeepAbove( false );
        KWin::clearState( winId(), NET::KeepAbove );

        m_config->setKeepBelow( false );
        KWin::clearState( winId(), NET::KeepBelow );
    }
}

void KNote::slotUpdateShowInTaskbar()
{
    if ( !m_config->showInTaskbar() )
        KWin::setState( winId(), KWin::windowInfo(winId()).state() | NET::SkipTaskbar );
    else
        KWin::clearState( winId(), NET::SkipTaskbar );
}

void KNote::slotUpdateDesktopActions()
{
    NETRootInfo wm_root( qt_xdisplay(), NET::NumberOfDesktops | NET::DesktopNames );
    NETWinInfo wm_client( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMDesktop );

    QStringList desktops;
    desktops.append( i18n("&All Desktops") );
    desktops.append( QString::null );           // Separator

    int count = wm_root.numberOfDesktops();
    for ( int n = 1; n <= count; n++ )
        desktops.append( QString("&%1 %2").arg( n ).arg( QString::fromUtf8(wm_root.desktopName( n )) ) );

    m_toDesktop->setItems( desktops );

    if ( wm_client.desktop() == NETWinInfo::OnAllDesktops )
        m_toDesktop->setCurrentItem( 0 );
    else
        m_toDesktop->setCurrentItem( wm_client.desktop() + 1 ); // compensate for separator (+1)
}

void KNote::slotUpdateViewport( int /*x*/, int y )
{
    if ( s_ppOffset )
        updateBackground( y );
}

// -------------------- private methods -------------------- //

void KNote::toDesktop( int desktop )
{
    if ( desktop == 0 )
        return;

    if ( desktop == NETWinInfo::OnAllDesktops )
        KWin::setOnAllDesktops( winId(), true );
    else
        KWin::setOnDesktop( winId(), desktop );
}

void KNote::createFold()
{
    QPixmap fold( 15, 15 );
    QPainter foldp( &fold );
    foldp.setPen( Qt::NoPen );
    foldp.setBrush( palette().active().dark() );
    QPointArray foldpoints( 3 );
    foldpoints.putPoints( 0, 3, 0, 0, 14, 0, 0, 14 );
    foldp.drawPolygon( foldpoints );
    foldp.end();
    m_fold->setPixmap( fold );
}

void KNote::updateLabelAlignment()
{
    // if the name is too long to fit, left-align it, otherwise center it (#59028)
    QString labelText = m_label->text();
    if ( m_label->fontMetrics().boundingRect( labelText ).width() > m_label->width() )
        m_label->setAlignment( AlignLeft );
    else
        m_label->setAlignment( AlignHCenter );
}

void KNote::updateFocus()
{
    if ( hasFocus() )
    {
        m_label->setBackgroundColor( palette().active().shadow() );
        m_button->show();

        if ( !m_editor->isReadOnly() )
        {
            if ( m_tool && m_tool->isHidden() && m_editor->textFormat() == QTextEdit::RichText )
            {
                m_tool->show();
		m_editor->cornerWidget()->show();
                setGeometry( x(), y(), width(), height() + m_tool->height() );
            }
        }
        else if ( m_tool && !m_tool->isHidden() )
        {
            m_tool->hide();
	    m_editor->cornerWidget()->hide();
            setGeometry( x(), y(), width(), height() - m_tool->height() );
            updateLayout();     // to update the minimum height
        }

        m_fold->hide();
    }
    else
    {
        m_button->hide();
        m_editor->cornerWidget()->hide();

        if ( m_tool && !m_tool->isHidden() )
        {
            m_tool->hide();
            setGeometry( x(), y(), width(), height() - m_tool->height() );
            updateLayout();     // to update the minimum height
        }

        if ( s_ppOffset )
        {
            m_label->setBackgroundColor( palette().active().midlight() );
            m_fold->show();
        }
        else
            m_label->setBackgroundColor( palette().active().background() );
    }
}

void KNote::updateMask()
{
    if ( !s_ppOffset )
    {
        clearMask();
        return;
    }

    int w = width();
    int h = height();
    QRegion reg( 0, s_ppOffset, w, h - s_ppOffset );

    const QBitmap *pushpin_bitmap = m_pushpin->pixmap()->mask();
    QRegion pushpin_reg( *pushpin_bitmap );
    m_pushpin->setMask( pushpin_reg );
    pushpin_reg.translate( m_pushpin->x(), m_pushpin->y() );

    if ( !hasFocus() )
    {
        QPointArray foldpoints( 3 );
        foldpoints.putPoints( 0, 3, w-15, h, w, h-15, w, h );
        QRegion fold( foldpoints, false );
        setMask( reg.unite( pushpin_reg ).subtract( fold ) );
    }
    else
        setMask( reg.unite( pushpin_reg ) );
}

void KNote::updateBackground( int y_offset )
{
    if ( !s_ppOffset )
    {
        m_editor->setPaper( QBrush( colorGroup().background() ) );
        return;
    }

    int w = m_editor->visibleWidth();
    int h = m_editor->visibleHeight();

    // in case y_offset is not set, calculate y_offset as the content
    // y-coordinate of the top-left point of the viewport - which is essentially
    // the vertical scroll amount
    if ( y_offset == -1 )
        y_offset = m_editor->contentsY();

    y_offset = y_offset % h;

    QImage grad_img( w, h, 32 );
    QRgb rgbcol;
    QColor bg = palette().active().background();

    for ( int i = 0; i < h; ++i )
    {
        // if the scrollbar has moved, then adjust the gradient by the amount the
        // scrollbar moved -- so that the background gradient looks ok when tiled

        // the lightness is calculated as follows:
        // if i >= y, then lightness = 150 - (i-y)*75/h;
        // if i < y, then lightness = 150 - (i+h-y)*75/h

        int i_1 = 150 - 75 * ((i - y_offset + h) % h) / h;
        rgbcol = bg.light( i_1 ).rgb();
        for ( int j = 0; j < w; ++j )
            grad_img.setPixel( j, i, rgbcol );
    }

    // setPaletteBackgroundPixmap makes QTextEdit::color() stop working!!
    m_editor->setPaper( QBrush( Qt::black, QPixmap( grad_img ) ) );
}

void KNote::updateLayout()
{
    const int headerHeight = m_label->sizeHint().height();
    const int margin = m_editor->margin();
    bool closeLeft = false;

    m_kwinConf->setGroup( "Style" );
    if ( m_kwinConf->readBoolEntry( "CustomButtonPositions" ) )
        closeLeft = m_kwinConf->readEntry( "ButtonsOnLeft" ).find( 'X' ) > -1;

    if ( s_ppOffset )
    {
        if ( !m_editor->paper().pixmap() )  // just changed the style
            setColor( palette().active().foreground(), palette().active().background() );

        m_pushpin->show();
        setFrameStyle( Panel | Raised );

        if ( closeLeft )
            m_pushpin->move( width() - m_pushpin->width(), 0 );
        else
            m_pushpin->move( 0, 0 );
    }
    else
    {
        if ( m_editor->paper().pixmap() )  // just changed the style
            setColor( palette().active().foreground(), palette().active().background() );

        setFrameStyle( WinPanel | Raised );
        m_pushpin->hide();
        m_fold->hide();
    }

    m_button->setGeometry(
        closeLeft ? contentsRect().x() : contentsRect().width() - headerHeight,
        contentsRect().y() + s_ppOffset,
        headerHeight,
        headerHeight
    );

    m_label->setGeometry(
        contentsRect().x(), contentsRect().y() + s_ppOffset,
        contentsRect().width(), headerHeight
    );

    m_editor->setGeometry( QRect(
        QPoint( contentsRect().x(),
                contentsRect().y() + headerHeight + s_ppOffset ),
        QPoint( contentsRect().right(),
                contentsRect().bottom() - ( m_tool ? (m_tool->isHidden() ? 0 : m_tool->height()) : 0 ) )
    ) );

    if( m_tool ) {
      m_tool->setGeometry(
          contentsRect().x(),
          contentsRect().bottom() - m_tool->height() + 1,
          contentsRect().width(),
          m_tool->height()
      );
    }

    if ( s_ppOffset )
        m_fold->move( width() - 15, height() - 15 );

    setMinimumSize(
        m_editor->cornerWidget()->width() + margin*2,
        headerHeight + s_ppOffset + ( m_tool ? (m_tool->isHidden() ? 0 : m_tool->height() ) : 0 ) +
                m_editor->cornerWidget()->height() + margin*2
    );

    updateLabelAlignment();
    updateMask();
    updateBackground();
}

// -------------------- protected methods -------------------- //

void KNote::drawFrame( QPainter *p )
{
    QRect r = frameRect();
    r.setTop( s_ppOffset );
    if ( s_ppOffset )
        qDrawShadePanel( p, r, colorGroup(), false, lineWidth() );
    else
        qDrawWinPanel( p, r, colorGroup(), false );
}

void KNote::showEvent( QShowEvent * )
{
    if ( m_config->hideNote() )
    {
        // KWin does not preserve these properties for hidden windows
        slotUpdateKeepAboveBelow();
        slotUpdateShowInTaskbar();
        toDesktop( m_config->desktop() );
        move( m_config->position() );
        m_config->setHideNote( false );
    }
}

void KNote::resizeEvent( QResizeEvent *qre )
{
    QFrame::resizeEvent( qre );
    updateLayout();
}

void KNote::closeEvent( QCloseEvent * )
{
    slotClose();
}

void KNote::dragEnterEvent( QDragEnterEvent *e )
{
    if ( !m_config->readOnly() )
        e->accept( KColorDrag::canDecode( e ) );
}

void KNote::dropEvent( QDropEvent *e )
{
    if ( m_config->readOnly() )
        return;

    QColor bg;
    if ( KColorDrag::decode( e, bg ) )
        setColor( paletteForegroundColor(), bg );
}

bool KNote::focusNextPrevChild( bool )
{
    return true;
}

bool KNote::event( QEvent *ev )
{
    if ( ev->type() == QEvent::LayoutHint )
    {
        updateLayout();
        return true;
    }
    else
        return QFrame::event( ev );
}

bool KNote::eventFilter( QObject *o, QEvent *ev )
{
    if ( ev->type() == QEvent::DragEnter &&
         KColorDrag::canDecode( static_cast<QDragEnterEvent *>(ev) ) )
    {
        dragEnterEvent( static_cast<QDragEnterEvent *>(ev) );
        return true;
    }

    if ( ev->type() == QEvent::Drop &&
         KColorDrag::canDecode( static_cast<QDropEvent *>(ev) ) )
    {
        dropEvent( static_cast<QDropEvent *>(ev) );
        return true;
    }

    if ( o == m_label )
    {
        QMouseEvent *e = (QMouseEvent *)ev;

        if ( ev->type() == QEvent::MouseButtonDblClick )
	{
	    if( !m_editor->isReadOnly())
               slotRename();
        }
        if ( ev->type() == QEvent::MouseButtonPress &&
             (e->button() == LeftButton || e->button() == MidButton))
        {
            e->button() == LeftButton ? KWin::raiseWindow( winId() )
                                      : KWin::lowerWindow( winId() );

            XUngrabPointer( qt_xdisplay(), qt_x_time );
            NETRootInfo wm_root( qt_xdisplay(), NET::WMMoveResize );
            wm_root.moveResizeRequest( winId(), e->globalX(), e->globalY(), NET::Move );
            return true;
        }

#if KDE_IS_VERSION( 3, 5, 1 )
        if ( ev->type() == QEvent::MouseButtonRelease )
        {
            NETRootInfo wm_root( qt_xdisplay(), NET::WMMoveResize );
            wm_root.moveResizeRequest( winId(), e->globalX(), e->globalY(), NET::MoveResizeCancel );
            return false;
        }
#endif

        if ( m_menu && ( ev->type() == QEvent::MouseButtonPress )
            && ( e->button() == RightButton ) )
        {
            m_menu->popup( QCursor::pos() );
            return true;
        }

        return false;
    }

    if ( o == m_editor )
    {
        if ( ev->type() == QEvent::FocusOut )
        {
            QFocusEvent *fe = static_cast<QFocusEvent *>(ev);
            if ( fe->reason() != QFocusEvent::Popup &&
                 fe->reason() != QFocusEvent::Mouse )
            {
                updateFocus();
                if ( m_editor->isModified() ) {
			saveConfig();
                        if ( !m_blockEmitDataChanged )
                            saveData();
		}
            }
        }
        else if ( ev->type() == QEvent::FocusIn )
            updateFocus();

        return false;
    }

    if ( o == m_editor->viewport() )
    {
        if ( m_edit_menu &&
             ev->type() == QEvent::MouseButtonPress &&
             ((QMouseEvent *)ev)->button() == RightButton )
        {
            m_edit_menu->popup( QCursor::pos() );
            return true;
        }
    }

    return false;
}

void KNote::deleteWhenIdle()
{
  if ( m_busy <= 0 )
    deleteLater();
  else
    m_deleteWhenIdle = true;
}

void KNote::aboutToEnterEventLoop()
{
  ++m_busy;
}

void KNote::eventLoopLeft()
{
  --m_busy;
  if ( m_busy <= 0 && m_deleteWhenIdle )
    deleteLater();
}


#include "knote.moc"
#include "knotebutton.moc"
