/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef NOTE_DISPLAY_ATTRIBUTE_H
#define NOTE_DISPLAY_ATTRIBUTE_H

#include <Akonadi/Attribute>

#include <QColor>
#include <QSize>
#include <QFont>
#include <QPoint>

class NoteDisplayAttribute : public Akonadi::Attribute
{
public:
    NoteDisplayAttribute();
    ~NoteDisplayAttribute();

    QByteArray type() const;

    NoteDisplayAttribute* clone() const;

    QByteArray serialized() const;

    void deserialize( const QByteArray &data );

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;
    QColor foregroundColor() const;
    void setForegroundColor(const QColor &color);

    QSize size() const;
    void setSize(const QSize &size);

    bool rememberDesktop() const;
    void setRememberDesktop(bool b);
    void setTabSize(int value);
    int tabSize() const;


    void setFont(const QFont &f);
    QFont font() const;
    void setTitleFont(const QFont &f);
    QFont titleFont() const;

    void setDesktop(int v);
    int desktop() const;
    void setIsHidden(bool b);
    bool isHidden() const;
    void setPosition(const QPoint &pos);
    QPoint position() const;
    void setShowInTaskbar(bool b);
    bool showInTaskbar() const;
    void setKeepAbove(bool b);
    bool keepAbove() const;
    void setKeepBelow(bool b);
    bool keepBelow() const;

    bool autoIndent() const;
    void setAutoIndent(bool b);
private:
    QFont mFont;
    QFont mTitleFont;
    QColor mBackgroundColor;
    QColor mForegroundgroundColor;
    QSize mSize;
    QPoint mPosition;
    int mTabSize;
    int mDesktop;
    bool mRememberDesktop;
    bool mAutoIndent;
    bool mHide;
    bool mShowInTaskbar;
    bool mKeepAbove;
    bool mKeepBelove;
    bool mKeepBelow;
};

#endif
