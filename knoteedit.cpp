
#include <qdragobject.h>
#include <qtextstream.h>
#include <qfile.h>
#include <kdebug.h>
#include <kurl.h>

#include "knoteedit.h"

KNoteEdit::KNoteEdit( QWidget* parent, const char* name )
	: QMultiLineEdit( parent, name )
{
	setAcceptDrops( TRUE );
	setBackgroundMode( QWidget::PaletteBase );
	setFrameStyle( NoFrame );
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
	
		for( QString currline = input.readLine();
	    	 currline != QString::null;
	     	currline = input.readLine() )
		{
			//put currline in editor
			insertLine( currline );
		}
	} else
		kdDebug() << "could not open input file" << endl;
}

void KNoteEdit::dumpToFile( QString& filename )
{
	QFile outfile( filename );
	if( outfile.open( IO_WriteOnly ) )
	{
		QTextStream output( &outfile );
	
		for( int line = 0; line < numLines(); line++ )
		{
			output << textLine( line ) << endl;
		}
	} else
		kdDebug() << "could not open file to write to" << endl;
}

void KNoteEdit::setAutoIndentMode( bool newmode )
{
	m_autoIndentMode = newmode;
}


void KNoteEdit::keyPressEvent(QKeyEvent* e){
	if ( e->key() == Key_Tab )
	{
		int line, col;
		cursorPosition( &line, &col );
		insertAt( "\t", line, col );
		return;
	}

	if( e->key() == Key_Return || e->key() == Key_Enter )
	{
		mynewLine();
		return;
	}

	QMultiLineEdit::keyPressEvent(e);
}

void KNoteEdit::mouseDoubleClickEvent( QMouseEvent* e )
{
	QMultiLineEdit::mouseDoubleClickEvent(e);

	int line, column = 0;
	getCursorPosition( &line, &column );

	QString text = markedText();
	//  kbDebug() << line << column << text;
	
	//now try to open the marked text???
}


void KNoteEdit::mynewLine()
{
	if ( isReadOnly() )
		return;

	if( !m_autoIndentMode )
	{
		newLine();
		return;
	}

	//in auto indent mode
	int line,col;
	bool found_one = false;

	getCursorPosition( &line, &col );

	QString string1, string2;
	while( line >= 0 )
	{
		string1 = textLine( line );
		string2 = string1.stripWhiteSpace();

		if( !string2.isEmpty() )
		{

			string1 = prefixString( string1 );
			found_one = TRUE;
			break;
		}
        line--;
	}

	// string will now contain those whitespace characters that I need to insert
	// on the next line.

	if( found_one )
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
	while( i < len && string.at(i).isSpace() )
		returnstring += string.at( i++ );

	return returnstring;
}


void KNoteEdit::dragEnterEvent( QDragEnterEvent* event )
{
	event->accept( QUriDrag::canDecode(event) || QTextDrag::canDecode(event) );
}


void KNoteEdit::dragMoveEvent( QDragMoveEvent* event )
{
	if( QUriDrag::canDecode(event) )
	{
		event->accept();
	}
	else if ( QTextDrag::canDecode(event) )
	{
		QMultiLineEdit::dragMoveEvent(event);
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
		QMultiLineEdit::dropEvent( event );
	}
}
#include "knoteedit.moc"
