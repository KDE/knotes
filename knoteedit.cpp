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

#include <kdebug.h>

#include "knoteedit.h"

using namespace Qt3;

KNoteEdit::KNoteEdit( QWidget* parent, const char* name )
    : QTextEdit( parent, name )
{
    setAcceptDrops( TRUE );
    setBackgroundMode( QWidget::PaletteBase );
    setFrameStyle( NoFrame );
    setWordWrap( WidgetWidth );
    setWrapPolicy( AtWhiteSpace );
    setTextFormat( PlainText );

    connect( this, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()) );
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
    selectAll();
    setFont( font );
    selectAll( false );
}

void KNoteEdit::setAutoIndentMode( bool newmode )
{
    m_autoIndentMode = newmode;
}

void KNoteEdit::slotReturnPressed()
{
    if ( m_autoIndentMode )
        indent();
}

/*
void KNoteEdit::keyPressEvent( QKeyEvent* e )
{
    if ( e->key() == Key_Tab )
    {
        int line, col;
        cursorPosition( &line, &col );
        insertAt( "\t", line, col );
        return;
    }

    if ( e->key() == Key_Return || e->key() == Key_Enter )
    {
        mynewLine();
        return;
    }

    QTextEdit::keyPressEvent( e );
}

void KNoteEdit::mynewLine()
{
    if ( isReadOnly() )
        return;

    if ( !m_autoIndentMode )
    {
        newLine();
        return;
    }

    //in auto indent mode
    int line,col;
    bool found_one = false;

    getCursorPosition( &line, &col );

    QString string1, string2;
    while ( line >= 0 )
    {
        string1 = textLine( line );
        string2 = string1.stripWhiteSpace();

        if ( !string2.isEmpty() )
        {
            string1 = prefixString( string1 );
            found_one = TRUE;
            break;
        }
        line--;
    }

    // string will now contain those whitespace characters that I need to insert
    // on the next line.

    if ( found_one )
    {
        newLine();
        int line, col;
        cursorPosition(&line, &col);
        insertAt(string1, line, col);
    }
    else
        newLine();
}

QString KNoteEdit::prefixString( QString string )
{
    // This routine returns the whitespace before the first non white space
    // character in string. This is  used in mynewLine() for indent mode.
    // It is assumed that string contains at least one non whitespace character
    // ie \n \r \t \v \f and space
    QString returnstring;

    int len = string.length();
    int i = 0;
    while ( i < len && string.at(i).isSpace() )
        returnstring += string.at( i++ );

    return returnstring;
}*/


void KNoteEdit::dragEnterEvent( QDragEnterEvent* event )
{
    event->accept( QUriDrag::canDecode(event) || QTextDrag::canDecode(event) );
}

void KNoteEdit::dragMoveEvent( QDragMoveEvent* event )
{
    if ( QUriDrag::canDecode(event) )
    {
        event->accept();
    }
    else if ( QTextDrag::canDecode(event) )
    {
        QTextEdit::dragMoveEvent(event);
    }
}

void KNoteEdit::dropEvent( QDropEvent* event )
{
    QStringList list;

    if ( QUriDrag::decodeToUnicodeUris( event, list ) )
    {
        emit gotUrlDrop( list.first() );
    }
    else if ( QTextDrag::canDecode( event ) )
    {
        QTextEdit::dropEvent( event );
    }
}

#include "knoteedit.moc"
