/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2001, The KNotes Developers

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
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*******************************************************************/

#include <qdragobject.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qcolor.h>
#include <qfont.h>
#include <qdialog.h>
#include <qfontmetrics.h>

#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kpopupmenu.h>
#include <kcolordialog.h>
#include <kxmlgui.h>
#include <kxmlguiclient.h>

#include "knoteedit.h"


KNoteEdit::KNoteEdit( QWidget* parent, const char* name )
    : QTextEdit( parent, name )
{
    setAcceptDrops( true );
    setWordWrap( WidgetWidth );
    setWrapPolicy( AtWhiteSpace );

    KXMLGUIClient* client = dynamic_cast<KXMLGUIClient*>(parent);
    KActionCollection* actions = client->actionCollection();

    // create the actions for the RMB menu
    KAction* undo = KStdAction::undo( this, SLOT(undo()), actions );
    KAction* redo = KStdAction::redo( this, SLOT(redo()), actions );
    m_cut = KStdAction::cut( this, SLOT(cut()), actions );
    m_copy = KStdAction::copy( this, SLOT(copy()), actions );
    m_paste = KStdAction::paste( this, SLOT(paste()), actions );

    m_cut->setEnabled( false );
    m_copy->setEnabled( false );
    m_paste->setEnabled( false );

    connect( this, SIGNAL(undoAvailable(bool)), undo, SLOT(setEnabled(bool)) );
    connect( this, SIGNAL(redoAvailable(bool)), redo, SLOT(setEnabled(bool)) );
    connect( this, SIGNAL(copyAvailable(bool)), m_copy, SLOT(setEnabled(bool)) );

    new KAction( i18n("Clear"), "editclear", 0, this, SLOT(clear()), actions, "edit_clear" );
    KStdAction::selectAll( this, SLOT(selectAll()), actions );

    // create the actions modifying the text format
    m_textBold = new KToggleAction( i18n( "&Bold" ), "text_bold", CTRL + Key_B,
                                    actions, "format_bold" );
    m_textItalic = new KToggleAction( i18n( "&Italic" ), "text_italic", CTRL + Key_I,
                                      actions, "format_italic" );
    m_textUnderline = new KToggleAction( i18n( "&Underline" ), "text_under", CTRL + Key_U,
                                         actions, "format_underline" );

    connect( m_textBold, SIGNAL(toggled(bool)), this, SLOT(setBold(bool)) );
    connect( m_textItalic, SIGNAL(toggled(bool)), this, SLOT(setItalic(bool)) );
    connect( m_textUnderline, SIGNAL(toggled(bool)), this, SLOT(setUnderline(bool)) );

    m_textAlignLeft = new KToggleAction( i18n( "Align &Left" ), "text_left", CTRL + Key_L,
                                 this, SLOT( textAlignLeft() ),
                                 actions, "format_alignleft" );
    m_textAlignLeft->setChecked( true );
    m_textAlignCenter = new KToggleAction( i18n( "Align &Center" ), "text_center", CTRL + ALT + Key_C,
                                 this, SLOT( textAlignCenter() ),
                                 actions, "format_aligncenter" );
    m_textAlignRight = new KToggleAction( i18n( "Align &Right" ), "text_right", CTRL + ALT + Key_R,
                                 this, SLOT( textAlignRight() ),
                                 actions, "format_alignright" );
    m_textAlignBlock = new KToggleAction( i18n( "Align &Block" ), "text_block", CTRL + Key_J,
                                  this, SLOT( textAlignBlock() ),
                                  actions, "format_alignblock" );

    m_textAlignLeft->setExclusiveGroup( "align" );
    m_textAlignCenter->setExclusiveGroup( "align" );
    m_textAlignRight->setExclusiveGroup( "align" );
    m_textAlignBlock->setExclusiveGroup( "align" );


    m_textList = new KToggleAction( i18n( "List" ), "enumList", 0,
                                    this, SLOT( textList() ),
                                    actions, "format_list" );

    m_textList->setExclusiveGroup( "style" );

    m_textSuper = new KToggleAction( i18n( "Superscript" ), "super", 0,
                                     this, SLOT( textSuperScript() ),
                                     actions, "format_super" );
    m_textSub = new KToggleAction( i18n( "Subscript" ), "sub", 0,
                                   this, SLOT( textSubScript() ),
                                   actions, "format_sub" );

    m_textSuper->setExclusiveGroup( "valign" );
    m_textSub->setExclusiveGroup( "valign" );

    m_textIncreaseIndent = new KAction( i18n( "Increase Indent" ), "format_increaseindent", 0,
                                this, SLOT(textIncreaseIndent()),
                                actions, "format_increaseindent" );

    m_textDecreaseIndent = new KAction( i18n( "Decrease Indent" ), "format_decreaseindent", 0,
                                this, SLOT(textDecreaseIndent()),
                                actions, "format_decreaseindent" );

    QPixmap pix( 16, 16 );
    pix.fill( black );
    m_textColor = new KAction( i18n( "Text Color..." ), pix, 0, this,
                               SLOT(textColor()), actions, "format_color" );


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

void KNoteEdit::readFile( QString& filename )
{
    QFile infile( filename );
    if( infile.open( IO_ReadOnly ) )
    {
        QTextStream input( &infile );
        setText( input.read() );
        infile.close();
    } else
        kdDebug() << "could not open input file" << endl;

    setModified( false );
}

void KNoteEdit::dumpToFile( QString& filename ) const
{
    QFile outfile( filename );
    if( outfile.open( IO_WriteOnly ) )
    {
        QTextStream output( &outfile );
        output << text();
        outfile.close();
    } else
        kdDebug() << "could not open file to write to" << endl;
}

void KNoteEdit::setTextFont( QFont& font )
{
    if ( textFormat() == PlainText )
    {
        setSelectionAttributes( 1, colorGroup().background(), false );
        setSelection( 0, 0, length(), paragraphLength( length() ), 1 );
        setCurrentFont( font );
        removeSelection( 1 );
    }
    else
    {
        setFamily( font.family() );
        setPointSize( font.pointSize() );
    }
}

void KNoteEdit::setTextColor( QColor& color )
{
    if ( textFormat() == PlainText )
    {
        setSelectionAttributes( 1, colorGroup().background(), false );
        setSelection( 0, 0, length(), paragraphLength( length() ), 1 );
        setColor( color );
        removeSelection( 1 );
    }
    else
        setColor( color );

    QPixmap pix( 16, 16 );
    pix.fill( color );
    m_textColor->setIconSet( pix );
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

    QTextEdit::setTextFormat( f );
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
    if ( m_textList->isChecked() )
        setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDisc );
    else
        setParagType( QStyleSheetItem::DisplayBlock, QStyleSheetItem::ListDisc );
}

void KNoteEdit::textSuperScript()
{
    if ( m_textSuper->isChecked() )
        setVerticalAlignment( AlignSuperScript );
    else
        setVerticalAlignment( AlignNormal );
}

void KNoteEdit::textSubScript()
{
    if ( m_textSub->isChecked() )
        setVerticalAlignment( AlignSubScript );
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

void KNoteEdit::dragEnterEvent( QDragEnterEvent* event )
{
    event->accept( QUriDrag::canDecode(event) || QTextDrag::canDecode(event) );
}

void KNoteEdit::dragMoveEvent( QDragMoveEvent* event )
{
    if ( QUriDrag::canDecode(event) )
        event->accept();
    else if ( QTextDrag::canDecode(event) )
        QTextEdit::dragMoveEvent(event);
}

void KNoteEdit::dropEvent( QDropEvent* event )
{
    QStringList list;

    if ( QUriDrag::decodeToUnicodeUris( event, list ) )
        emit gotUrlDrop( list.first() );
    else if ( QTextDrag::canDecode( event ) )
        QTextEdit::dropEvent( event );
}

/** private slots **/

void KNoteEdit::slotReturnPressed()
{
    if ( m_autoIndentMode )
        autoIndent();
}

void KNoteEdit::fontChanged( const QFont &f )
{
//TODO
//    m_comboFont->lineEdit()->setText( f.family() );
//    m_comboSize->lineEdit()->setText( QString::number( f.pointSize() ) );
    m_textBold->setChecked( f.bold() );
    m_textItalic->setChecked( f.italic() );
    m_textUnderline->setChecked( f.underline() );
}

void KNoteEdit::colorChanged( const QColor &c )
{
    QPixmap pix( 16, 16 );
    pix.fill( c );
    m_textColor->setIconSet( pix );
}

void KNoteEdit::alignmentChanged( int a )
{
    // TODO: AlignAuto
    if ( ( a == AlignAuto ) || ( a & AlignLeft ) )
        m_textAlignLeft->setChecked( true );
    else if ( ( a & AlignHCenter ) )
        m_textAlignCenter->setChecked( true );
    else if ( ( a & AlignRight ) )
        m_textAlignRight->setChecked( true );
    else if ( ( a & AlignJustify ) )
        m_textAlignBlock->setChecked( true );
}

void KNoteEdit::verticalAlignmentChanged( VerticalAlignment a )
{
    if ( a == AlignNormal )
    {
        m_textSuper->setChecked( false );
        m_textSub->setChecked( false );
    }
    else if ( a == AlignSuperScript )
        m_textSuper->setChecked( true );
    else if ( a == AlignSubScript )
        m_textSub->setChecked( true );
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
