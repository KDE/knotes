/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2003, The KNotes Developers

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

#include <qdragobject.h>
#include <qfile.h>
#include <qlayout.h>
#include <qbuttongroup.h>

#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>
#include <kurldrag.h>
#include <kstdaction.h>
#include <kcolordialog.h>
#include <kxmlguiclient.h>

#include <assert.h>

#include "knoteedit.h"
#include "knotebutton.h"

static const short SEP = 5;
static const short ICON_SIZE = 10;


KNoteEdit::KNoteEdit( QWidget *tool, QWidget* parent, const char* name )
    : KTextEdit( parent, name )
{
    setAcceptDrops( true );
    setWordWrap( WidgetWidth );
    setWrapPolicy( AtWhiteSpace );

    KXMLGUIClient* client = dynamic_cast<KXMLGUIClient*>(parent);
    assert(client);
    KActionCollection* actions = client->actionCollection();


    // create the actions for the RMB menu
    KAction* undo = KStdAction::undo( this, SLOT(undo()), actions );
    KAction* redo = KStdAction::redo( this, SLOT(redo()), actions );
    undo->setEnabled( isUndoAvailable() );
    redo->setEnabled( isRedoAvailable() );

    m_cut = KStdAction::cut( this, SLOT(cut()), actions );
    m_copy = KStdAction::copy( this, SLOT(copy()), actions );
    m_paste = KStdAction::paste( this, SLOT(paste()), actions );

    m_cut->setEnabled( false );
    m_copy->setEnabled( false );
    m_paste->setEnabled( true );

    connect( this, SIGNAL(undoAvailable(bool)), undo, SLOT(setEnabled(bool)) );
    connect( this, SIGNAL(redoAvailable(bool)), redo, SLOT(setEnabled(bool)) );

    connect( this, SIGNAL(copyAvailable(bool)), m_cut, SLOT(setEnabled(bool)) );
    connect( this, SIGNAL(copyAvailable(bool)), m_copy, SLOT(setEnabled(bool)) );

    new KAction( i18n("Clear"), "editclear", 0, this, SLOT(clear()), actions, "edit_clear" );
    KStdAction::selectAll( this, SLOT(selectAll()), actions );

    // create the tool buttons (can't use actions yet :-( )
    QBoxLayout *layout = new QBoxLayout( tool, QBoxLayout::LeftToRight );

    m_textBold = new KNoteButton( "text_bold", tool );
    m_textBold->setToggleButton( true );
    connect( m_textBold, SIGNAL(clicked()), this, SLOT(slotSetBold()) );
    layout->addWidget( m_textBold );

    m_textItalic = new KNoteButton( "text_italic", tool );
    m_textItalic->setToggleButton( true );
    connect( m_textItalic, SIGNAL(clicked()), this, SLOT(slotSetItalic()) );
    layout->addWidget( m_textItalic );

    m_textUnderline = new KNoteButton( "text_under", tool );
    m_textUnderline->setToggleButton( true );
    connect( m_textUnderline, SIGNAL(clicked()), this, SLOT(slotSetUnderline()) );
    layout->addWidget( m_textUnderline );

    layout->addSpacing( SEP );

    m_textAlignLeft = new KNoteButton( "text_left", tool );
    m_textAlignLeft->setToggleButton( true );
    connect( m_textAlignLeft, SIGNAL(clicked()), this, SLOT(textAlignLeft()) );
    layout->addWidget( m_textAlignLeft );

    m_textAlignCenter = new KNoteButton( "text_center", tool );
    m_textAlignCenter->setToggleButton( true );
    connect( m_textAlignCenter, SIGNAL(clicked()), this, SLOT(textAlignCenter()) );
    layout->addWidget( m_textAlignCenter );

    m_textAlignRight = new KNoteButton( "text_right", tool );
    m_textAlignRight->setToggleButton( true );
    connect( m_textAlignRight, SIGNAL(clicked()), this, SLOT(textAlignRight()) );
    layout->addWidget( m_textAlignRight );

    m_textAlignBlock = new KNoteButton( "text_block", tool );
    m_textAlignBlock->setToggleButton( true );
    connect( m_textAlignBlock, SIGNAL(clicked()), this, SLOT(textAlignBlock()) );
    layout->addWidget( m_textAlignBlock );

    QButtonGroup *align = new QButtonGroup( this );
    align->setExclusive( true );
    align->hide();
    align->insert( m_textAlignLeft );
    align->insert( m_textAlignCenter );
    align->insert( m_textAlignRight );
    align->insert( m_textAlignBlock );

    m_textAlignLeft->setOn( true );  // ???? TODO: really always true?

    layout->addSpacing( SEP );

    m_textList = new KNoteButton( "enum_list", tool );
    m_textList->setToggleButton( true );
    connect( m_textList, SIGNAL(clicked()), this, SLOT(textList()) );
    layout->addWidget( m_textList );

    layout->addSpacing( SEP );

    m_textSuper = new KNoteButton( "text_super", tool );
    m_textSuper->setToggleButton( true );
    connect( m_textSuper, SIGNAL(clicked()), this, SLOT(textSuperScript()) );
    layout->addWidget( m_textSuper );

    m_textSub = new KNoteButton( "text_sub", tool );
    m_textSub->setToggleButton( true );
    connect( m_textSub, SIGNAL(clicked()), this, SLOT(textSubScript()) );
    layout->addWidget( m_textSub );

    layout->addSpacing( SEP );

    m_textIncreaseIndent = new KNoteButton( "format_increaseindent", tool );
    connect( m_textIncreaseIndent, SIGNAL(clicked()), this, SLOT(textIncreaseIndent()) );
    layout->addWidget( m_textIncreaseIndent );

    m_textDecreaseIndent = new KNoteButton( "format_decreaseindent", tool );
    connect( m_textDecreaseIndent, SIGNAL(clicked()), this, SLOT(textDecreaseIndent()) );
    layout->addWidget( m_textDecreaseIndent );

    layout->addSpacing( SEP );

    QPixmap pix( ICON_SIZE, ICON_SIZE );
    pix.fill( black );                  // ??? TODO: really always black?
    m_textColor = new KNoteButton( QString::null, tool );
    m_textColor->setIconSet( pix );
    connect( m_textColor, SIGNAL(clicked()), this, SLOT(textColor()) );
    layout->addWidget( m_textColor );

    layout->addStretch( 1 );

    connect( this, SIGNAL(returnPressed()), SLOT(slotReturnPressed()) );
    connect( this, SIGNAL(currentFontChanged( const QFont & )),
             this, SLOT(fontChanged( const QFont & )) );
    connect( this, SIGNAL(currentColorChanged( const QColor & )),
             this, SLOT(colorChanged( const QColor & )) );
    connect( this, SIGNAL(currentAlignmentChanged( int )),
             this, SLOT(alignmentChanged( int )) );
    connect( this, SIGNAL(currentVerticalAlignmentChanged( VerticalAlignment )),
             this, SLOT(verticalAlignmentChanged( VerticalAlignment )) );
}

KNoteEdit::~KNoteEdit()
{
}

void KNoteEdit::setTextFont( const QFont& font )
{
    if ( textFormat() == PlainText )
        setFont( font );
    else
        setCurrentFont( font );
}

void KNoteEdit::setTextColor( const QColor& color )
{
    setColor( color );
    colorChanged( color );
}

void KNoteEdit::setTabStop( int tabs )
{
    QFontMetrics fm( font() );
    setTabStopWidth( fm.width( 'x' ) * tabs );
}

void KNoteEdit::setAutoIndentMode( bool newmode )
{
    m_autoIndentMode = newmode;
}


/** public slots **/

void KNoteEdit::setTextFormat( TextFormat f )
{
    if ( f == RichText )
        enableRichTextActions();
    else
        disableRichTextActions();

    KTextEdit::setTextFormat( f );
}

void KNoteEdit::textColor()
{
    QColor c = color();
    int ret = KColorDialog::getColor( c, this );
    if ( ret == QDialog::Accepted )
        setTextColor( c );
}

void KNoteEdit::textAlignLeft()
{
    setAlignment( AlignLeft );
}

void KNoteEdit::textAlignCenter()
{
    setAlignment( AlignCenter );
}

void KNoteEdit::textAlignRight()
{
    setAlignment( AlignRight );
}

void KNoteEdit::textAlignBlock()
{
    setAlignment( AlignJustify );
}

void KNoteEdit::textList()
{
    if ( m_textList->isOn() )
        setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDisc );
    else
        setParagType( QStyleSheetItem::DisplayBlock, QStyleSheetItem::ListDisc );
}

void KNoteEdit::textSuperScript()
{
    if ( m_textSuper->isOn() )
    {
        m_textSub->setOn( false );
        setVerticalAlignment( AlignSuperScript );
    }
    else
        setVerticalAlignment( AlignNormal );
}

void KNoteEdit::textSubScript()
{
    if ( m_textSub->isOn() )
    {
        m_textSuper->setOn( false );
        setVerticalAlignment( AlignSubScript );
    }
    else
        setVerticalAlignment( AlignNormal );
}

void KNoteEdit::textIncreaseIndent()
{
}

void KNoteEdit::textDecreaseIndent()
{
}


/** protected methods **/

void KNoteEdit::contentsDragEnterEvent( QDragEnterEvent* event )
{
    if ( KURLDrag::canDecode( event ) )
        event->accept();
    else
        KTextEdit::contentsDragEnterEvent( event );
}

void KNoteEdit::contentsDragMoveEvent( QDragMoveEvent* event )
{
    if ( KURLDrag::canDecode( event ) )
        event->accept();
    else
        KTextEdit::contentsDragMoveEvent( event );
}

void KNoteEdit::contentsDropEvent( QDropEvent* event )
{
    KURL::List list;

    if ( KURLDrag::decode( event, list ) )
        for ( KURL::List::Iterator it = list.begin(); it != list.end(); ++it )
        {
            if ( it != list.begin() )
                insert( ", " );

            insert( (*it).prettyURL() );
        }
    else
        KTextEdit::contentsDropEvent( event );
}

/** private slots **/

void KNoteEdit::slotReturnPressed()
{
    if ( m_autoIndentMode )
        autoIndent();
}

void KNoteEdit::slotSetBold()
{
    setBold( m_textBold->isOn() );
}

void KNoteEdit::slotSetItalic()
{
    setItalic( m_textItalic->isOn() );
}

void KNoteEdit::slotSetUnderline()
{
    setUnderline( m_textUnderline->isOn() );
}

void KNoteEdit::fontChanged( const QFont &f )
{
//TODO
//    m_comboFont->lineEdit()->setText( f.family() );
//    m_comboSize->lineEdit()->setText( QString::number( f.pointSize() ) );

    m_textBold->setOn( f.bold() );
    m_textItalic->setOn( f.italic() );
    m_textUnderline->setOn( f.underline() );
}

void KNoteEdit::colorChanged( const QColor &c )
{
    QPixmap pix( ICON_SIZE, ICON_SIZE );
    pix.fill( c );
    m_textColor->setIconSet( pix );
}

void KNoteEdit::alignmentChanged( int a )
{
    // TODO: AlignAuto
    if ( ( a == AlignAuto ) || ( a & AlignLeft ) )
        m_textAlignLeft->setOn( true );
    else if ( ( a & AlignHCenter ) )
        m_textAlignCenter->setOn( true );
    else if ( ( a & AlignRight ) )
        m_textAlignRight->setOn( true );
    else if ( ( a & AlignJustify ) )
        m_textAlignBlock->setOn( true );
}

void KNoteEdit::verticalAlignmentChanged( VerticalAlignment a )
{
    if ( a == AlignNormal )
    {
        m_textSuper->setOn( false );
        m_textSub->setOn( false );
    }
    else if ( a == AlignSuperScript )
        m_textSuper->setOn( true );
    else if ( a == AlignSubScript )
        m_textSub->setOn( true );
}


/** private methods **/

void KNoteEdit::autoIndent()
{
    int para, index;
    QString string;
    getCursorPosition( &para, &index );
    while ( para > 0 && string.stripWhiteSpace().isEmpty() )
        string = text( --para );

    if ( string.stripWhiteSpace().isEmpty() )
        return;

    // This routine returns the whitespace before the first non white space
    // character in string.
    // It is assumed that string contains at least one non whitespace character
    // ie \n \r \t \v \f and space
    QString indentString;

    int len = string.length();
    int i = 0;
    while ( i < len && string.at(i).isSpace() )
        indentString += string.at( i++ );

    if ( !indentString.isEmpty() )
        insert( indentString );
}

void KNoteEdit::emitLinkClicked( const QString &s )
{
    kdDebug() << k_funcinfo << s << endl;
}

void KNoteEdit::enableRichTextActions()
{
    m_textColor->setEnabled( true );

    m_textBold->setEnabled( true );
    m_textItalic->setEnabled( true );
    m_textUnderline->setEnabled( true );

    m_textAlignLeft->setEnabled( true );
    m_textAlignCenter->setEnabled( true );
    m_textAlignRight->setEnabled( true );
    m_textAlignBlock->setEnabled( true );

    m_textList->setEnabled( true );
    m_textSuper->setEnabled( true );
    m_textSub->setEnabled( true );

    m_textIncreaseIndent->setEnabled( true );
    m_textDecreaseIndent->setEnabled( true );
}

void KNoteEdit::disableRichTextActions()
{
    m_textColor->setEnabled( false );

    m_textBold->setEnabled( false );
    m_textItalic->setEnabled( false );
    m_textUnderline->setEnabled( false );

    m_textAlignLeft->setEnabled( false );
    m_textAlignCenter->setEnabled( false );
    m_textAlignRight->setEnabled( false );
    m_textAlignBlock->setEnabled( false );

    m_textList->setEnabled( false );
    m_textSuper->setEnabled( false );
    m_textSub->setEnabled( false );

    m_textIncreaseIndent->setEnabled( false );
    m_textDecreaseIndent->setEnabled( false );
}

#include "knoteedit.moc"
