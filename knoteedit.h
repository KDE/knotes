/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2002, The KNotes Developers

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

#ifndef KNOTEEDIT_H
#define KNOTEEDIT_H

#include <qwidget.h>

#include <ktextedit.h>

class QFont;
class QColor;
class KAction;
class KToggleAction;


class KNoteEdit : public KTextEdit
{
    Q_OBJECT
public:
    KNoteEdit( QWidget *parent=0, const char *name=0 );
    ~KNoteEdit();

    void readFile( QString& filename );
    void dumpToFile( QString& filename ) const;
    
    void setTextFont( QFont& font );
    void setTextColor( QColor& color );
    void setTabStop( int tabs );
    void setAutoIndentMode( bool newmode );

public slots:
    virtual void setTextFormat( TextFormat f );

//    void textStyleSelected( int );
//    void textSizeSelected( int );
//    void textFontSelected( const QString & );

    void textColor();

    void textAlignLeft();
    void textAlignCenter();
    void textAlignRight();
    void textAlignBlock();

    void textList();

    void textSuperScript();
    void textSubScript();

    void textIncreaseIndent();
    void textDecreaseIndent();

signals:
    void gotUrlDrop( const QString& url );

protected:
    virtual void contentsDragEnterEvent( QDragEnterEvent *e );
    virtual void contentsDragMoveEvent( QDragMoveEvent *e );
    virtual void contentsDropEvent( QDropEvent *e );

private slots:
    void slotReturnPressed();

    void fontChanged( const QFont &f );
    void colorChanged( const QColor &c );
    void alignmentChanged( int a );
    void verticalAlignmentChanged( VerticalAlignment a );

private:
    void autoIndent();

    void enableRichTextActions();
    void disableRichTextActions();


    KAction *m_cut;
    KAction *m_copy;
    KAction *m_paste;

    KAction *m_textColor;

    KToggleAction *m_textBold;
    KToggleAction *m_textItalic;
    KToggleAction *m_textUnderline;

    KToggleAction *m_textAlignLeft;
    KToggleAction *m_textAlignCenter;
    KToggleAction *m_textAlignRight;
    KToggleAction *m_textAlignBlock;

    KToggleAction *m_textList;
    KToggleAction *m_textSuper;
    KToggleAction *m_textSub;

    KAction *m_textIncreaseIndent;
    KAction *m_textDecreaseIndent;

    bool m_autoIndentMode;
};

#endif
