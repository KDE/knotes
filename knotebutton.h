
#ifndef KNOTEBUTTON_H
#define KNOTEBUTTON_H

#include <qpushbutton.h>
#include <qdrawutil.h>

#include <iostream.h>

class KNoteButton: public QPushButton {
	Q_OBJECT
public:
	KNoteButton( QWidget* parent=0, const char* name=0 )
		: QPushButton( parent, name )
	{
		setFocusPolicy( NoFocus );
		flat = true;
		last_button = 0;
	}
	~KNoteButton() {}

	bool flat;
	int  last_button;
	
protected:
	void enterEvent( QEvent * )
	{
		flat = false;
		repaint( false );
	}
	
	void leaveEvent( QEvent * )
	{
		flat = true;
		repaint();
	}
	
	void mousePressEvent( QMouseEvent *e)
	{
		if ( isDown())
			return;

		bool hit = hitButton( e->pos() );
		if ( hit ){
			last_button = e->button();
			setDown( TRUE );
			repaint( FALSE );
			emit pressed();
		}
	}
	
	void mouseReleaseEvent( QMouseEvent *e)
	{
		if ( !isDown() )
		{
			last_button = 0;
			return;
		}
		
		bool hit = hitButton( e->pos() );
		setDown( FALSE );
		if ( hit )
		{
			if ( isToggleButton() )
				setOn( !isOn() );
			repaint( FALSE );
			
			if ( isToggleButton() )
				emit toggled( isOn() );
			emit released();
			emit clicked();
		}
		else
		{
			repaint();
			emit released();
		}
		last_button = 0;
	}
	
	void mouseMoveEvent( QMouseEvent *e)
	{
		if (!last_button)
			return;
	
		if ( !(e->state() & LeftButton) &&
		     !(e->state() & MidButton) &&
		     !(e->state() & RightButton))
			return;

    	bool hit = hitButton( e->pos() );
		if ( hit )
		{
			if ( !isDown() )
			{
				setDown( true );
				repaint( false );
				emit pressed();
			}
		} else
		{
			if ( isDown() )
			{
				setDown( false );
				repaint();
				emit released();
			}
		}
	}
	
	void paint( QPainter* painter )
	{
		if( painter == NULL ) cerr << "painter is null" << endl;
		if( isDown() || (isOn() && !flat) )
		{
			if ( style() == WindowsStyle )
				qDrawWinButton( painter, 0, 0, width(),
				                height(), colorGroup(), TRUE );
			else
				qDrawShadePanel( painter, 0, 0, width(),
				                 height(), colorGroup(), TRUE, 2, 0L );
		}
		else if( !flat )
		{
			if ( style() == WindowsStyle )
				qDrawWinButton( painter, 0, 0, width(), height(),
				                colorGroup(), FALSE );
			else
				qDrawShadePanel( painter, 0, 0, width(), height(),
				                 colorGroup(), FALSE, 2, 0L );
		}
		
		int dx = ( width() - pixmap()->width() ) / 2;
		int dy = ( height() - pixmap()->height() ) / 2;
		if ( isDown() && style() == WindowsStyle )
		{
			dx++;
			dy++;
		}
		if( painter )
			painter->drawPixmap( dx, dy, *pixmap());
		else cerr << "painter is NULL" << endl;
	}

	void drawButton( QPainter* p )
	{
		paint( p );
	}
	
	void drawButtonLabel( QPainter* p )
	{
		paint( p );
	}
};


#endif
