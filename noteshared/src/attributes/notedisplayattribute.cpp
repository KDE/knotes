/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "notedisplayattribute.h"

#include <QByteArray>
#include <QDataStream>
#include <QFontDatabase>

using namespace NoteShared;

NoteDisplayAttribute::NoteDisplayAttribute()
    : Akonadi::Attribute()
    , mFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont))
    , mTitleFont(QFontDatabase::systemFont(QFontDatabase::TitleFont))
    //@krazy:cond=qenums because everyone expects yellow postit notes
    , mBackgroundColor(Qt::yellow)
    , mForegroundgroundColor(Qt::black)
    //@krazy:endcond=qenums
    , mSize(300, 300)
    , mPosition(QPoint(-10000, -10000))
    , mTabSize(4)
    , mDesktop(-10)
    , mRememberDesktop(true)
    , mAutoIndent(true)
    , mHide(false)
    , mShowInTaskbar(false)
    , mKeepAbove(false)
    , mKeepBelove(false)
    , mKeepBelow(false)
{
}

NoteDisplayAttribute::~NoteDisplayAttribute()
{
}

NoteDisplayAttribute *NoteDisplayAttribute::clone() const
{
    auto attr = new NoteDisplayAttribute();
    attr->setBackgroundColor(backgroundColor());
    attr->setForegroundColor(foregroundColor());
    attr->setSize(size());
    attr->setRememberDesktop(rememberDesktop());
    attr->setTabSize(tabSize());
    attr->setFont(font());
    attr->setTitleFont(titleFont());
    attr->setDesktop(desktop());
    attr->setIsHidden(isHidden());
    attr->setPosition(position());
    attr->setShowInTaskbar(showInTaskbar());
    attr->setKeepAbove(keepAbove());
    attr->setKeepBelow(keepBelow());
    attr->setAutoIndent(autoIndent());
    return attr;
}

void NoteDisplayAttribute::deserialize(const QByteArray &data)
{
    QDataStream s(data);
    s.setVersion(QDataStream::Qt_5_11);
    s >> mFont;
    s >> mTitleFont;
    s >> mBackgroundColor;
    s >> mForegroundgroundColor;
    s >> mSize;
    s >> mPosition;
    s >> mTabSize;
    s >> mDesktop;
    s >> mRememberDesktop;
    s >> mAutoIndent;
    s >> mHide;
    s >> mShowInTaskbar;
    s >> mKeepAbove;
    s >> mKeepBelove;
    s >> mKeepBelow;
}

QByteArray NoteDisplayAttribute::serialized() const
{
    QByteArray result;
    QDataStream s(&result, QIODevice::WriteOnly);
    s.setVersion(QDataStream::Qt_5_11);
    s << mFont;
    s << mTitleFont;
    s << mBackgroundColor;
    s << mForegroundgroundColor;
    s << mSize;
    s << mPosition;
    s << mTabSize;
    s << mDesktop;
    s << mRememberDesktop;
    s << mAutoIndent;
    s << mHide;
    s << mShowInTaskbar;
    s << mKeepAbove;
    s << mKeepBelove;
    s << mKeepBelow;
    return result;
}

QByteArray NoteDisplayAttribute::type() const
{
    static const QByteArray sType("NoteDisplayAttribute");
    return sType;
}

void NoteDisplayAttribute::setBackgroundColor(const QColor &color)
{
    mBackgroundColor = color;
}

QColor NoteDisplayAttribute::backgroundColor() const
{
    return mBackgroundColor;
}

void NoteDisplayAttribute::setForegroundColor(const QColor &color)
{
    mForegroundgroundColor = color;
}

QSize NoteDisplayAttribute::size() const
{
    return mSize;
}

void NoteDisplayAttribute::setSize(const QSize &size)
{
    mSize = size;
}

QColor NoteDisplayAttribute::foregroundColor() const
{
    return mForegroundgroundColor;
}

bool NoteDisplayAttribute::rememberDesktop() const
{
    return mRememberDesktop;
}

void NoteDisplayAttribute::setRememberDesktop(bool b)
{
    mRememberDesktop = b;
}

int NoteDisplayAttribute::tabSize() const
{
    return mTabSize;
}

void NoteDisplayAttribute::setTabSize(int value)
{
    mTabSize = value;
}

bool NoteDisplayAttribute::autoIndent() const
{
    return mAutoIndent;
}

void NoteDisplayAttribute::setAutoIndent(bool b)
{
    mAutoIndent = b;
}

void NoteDisplayAttribute::setFont(const QFont &f)
{
    mFont = f;
}

QFont NoteDisplayAttribute::font() const
{
    return mFont;
}

void NoteDisplayAttribute::setTitleFont(const QFont &f)
{
    mTitleFont = f;
}

QFont NoteDisplayAttribute::titleFont() const
{
    return mTitleFont;
}

void NoteDisplayAttribute::setDesktop(int v)
{
    mDesktop = v;
}

int NoteDisplayAttribute::desktop() const
{
    return mDesktop;
}

void NoteDisplayAttribute::setIsHidden(bool b)
{
    mHide = b;
}

bool NoteDisplayAttribute::isHidden() const
{
    return mHide;
}

void NoteDisplayAttribute::setPosition(const QPoint &pos)
{
    mPosition = pos;
}

QPoint NoteDisplayAttribute::position() const
{
    return mPosition;
}

void NoteDisplayAttribute::setShowInTaskbar(bool b)
{
    mShowInTaskbar = b;
}

bool NoteDisplayAttribute::showInTaskbar() const
{
    return mShowInTaskbar;
}

void NoteDisplayAttribute::setKeepAbove(bool b)
{
    mKeepAbove = b;
}

bool NoteDisplayAttribute::keepAbove() const
{
    return mKeepAbove;
}

void NoteDisplayAttribute::setKeepBelow(bool b)
{
    mKeepBelow = b;
}

bool NoteDisplayAttribute::keepBelow() const
{
    return mKeepBelow;
}

bool NoteDisplayAttribute::operator==(const NoteDisplayAttribute &other) const
{
    return (backgroundColor() == other.backgroundColor()) && (foregroundColor() == other.foregroundColor()) && (size() == other.size())
        && (rememberDesktop() == other.rememberDesktop()) && (tabSize() == other.tabSize()) && (font() == other.font()) && (titleFont() == other.titleFont())
        && (desktop() == other.desktop()) && (isHidden() == other.isHidden()) && (position() == other.position()) && (showInTaskbar() == other.showInTaskbar())
        && (keepAbove() == other.keepAbove()) && (keepBelow() == other.keepBelow()) && (autoIndent() == other.autoIndent());
}
