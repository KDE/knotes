

#ifndef KNOTEEDIT_H
#define KNOTEEDIT_H

#include <qwidget.h>
#include <qmultilineedit.h>


class KNoteEdit : public QMultiLineEdit{
   Q_OBJECT
public: 
	KNoteEdit( QWidget *parent=0, const char *name=0 );
	~KNoteEdit();
	
	void setAutoIndentMode( bool newmode );
	void readFile( QString& filename );
	void dumpToFile( QString& filename );
	
protected:
	void  dragMoveEvent(QDragMoveEvent* event);
	void  dragEnterEvent(QDragEnterEvent* event);
	void  dropEvent(QDropEvent* event);

	void  mouseDoubleClickEvent ( QMouseEvent * e );
	void  keyPressEvent(QKeyEvent *e);
	void  mynewLine();

private:
	QString prefixString( QString string );
	
	bool m_autoIndentMode;

signals:
	void gotUrlDrop(const char* url);

};

#endif
