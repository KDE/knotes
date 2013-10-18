/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2005, The KNotes Developers

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

#ifndef KNOTEEDIT_H
#define KNOTEEDIT_H

#include "knotes_export.h"
#include <QTextCharFormat>
#include <QWidget>

#include "pimcommon/widgets/customtextedit.h"

class QFont;

class KAction;
class KActionCollection;
class KFontAction;
class KFontSizeAction;
class KToggleAction;
class KNote;

class KNOTES_EXPORT KNoteEdit : public PimCommon::CustomTextEdit
{
    Q_OBJECT
public:
    explicit KNoteEdit(const QString &configFile, KActionCollection *actions, QWidget *parent = 0 );
    ~KNoteEdit();

    void setNote( KNote *_note ) {
        m_note = _note;
    }
    void setText( const QString &text );
    QString text() const;

    void setTextFont( const QFont &font );
    void setTabStop( int tabs );
    void setAutoIndentMode( bool newmode );

public slots:
    void setRichText( bool );

    void textBold( bool );
    void textStrikeOut( bool );

    void slotTextColor();

    void textAlignLeft();
    void textAlignCenter();
    void textAlignRight();
    void textAlignBlock();

    void textList();

    void textSuperScript();
    void textSubScript();

    void textIncreaseIndent();
    void textDecreaseIndent();
    void setTextFontSize( int );

    void slotTextBackgroundColor();
    void slotInsertDate();

protected:
    virtual void keyPressEvent( QKeyEvent * );
    virtual void focusInEvent( QFocusEvent * );
    virtual void focusOutEvent( QFocusEvent * );

protected slots:
    void mousePopupMenuImplementation(const QPoint& pos);

private slots:
    void slotCurrentCharFormatChanged( const QTextCharFormat & );
    void slotCursorPositionChanged();
    void slotUpperCase();
    void slotLowerCase();

private:
    void autoIndent();

    void setTextFormat( const QTextCharFormat & );

    void enableRichTextActions(bool enabled);

private:

    KToggleAction *m_textBold;
    KToggleAction *m_textItalic;
    KToggleAction *m_textUnderline;
    KToggleAction *m_textStrikeOut;

    KToggleAction *m_textAlignLeft;
    KToggleAction *m_textAlignCenter;
    KToggleAction *m_textAlignRight;
    KToggleAction *m_textAlignBlock;

    KToggleAction *m_textList;
    KToggleAction *m_textSuper;
    KToggleAction *m_textSub;

    KAction       *m_textIncreaseIndent;
    KAction       *m_textDecreaseIndent;

    KAction         *m_textColor;
    KFontAction     *m_textFont;
    KFontSizeAction *m_textSize;
    KNote           *m_note;
    KActionCollection *m_actions;
    bool m_autoIndentMode;
};

#endif
