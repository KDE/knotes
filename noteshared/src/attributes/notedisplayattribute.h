/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <AkonadiCore/Attribute>

#include <QColor>
#include <QFont>
#include <QPoint>
#include <QSize>

namespace NoteShared
{
class NOTESHARED_EXPORT NoteDisplayAttribute : public Akonadi::Attribute
{
public:
    NoteDisplayAttribute();
    ~NoteDisplayAttribute() override;

    Q_REQUIRED_RESULT QByteArray type() const override;

    NoteDisplayAttribute *clone() const override;

    Q_REQUIRED_RESULT QByteArray serialized() const override;

    void deserialize(const QByteArray &data) override;

    void setBackgroundColor(const QColor &color);
    Q_REQUIRED_RESULT QColor backgroundColor() const;
    Q_REQUIRED_RESULT QColor foregroundColor() const;
    void setForegroundColor(const QColor &color);

    Q_REQUIRED_RESULT QSize size() const;
    void setSize(const QSize &size);

    Q_REQUIRED_RESULT bool rememberDesktop() const;
    void setRememberDesktop(bool b);
    void setTabSize(int value);
    Q_REQUIRED_RESULT int tabSize() const;

    void setFont(const QFont &f);
    Q_REQUIRED_RESULT QFont font() const;
    void setTitleFont(const QFont &f);
    Q_REQUIRED_RESULT QFont titleFont() const;

    void setDesktop(int v);
    Q_REQUIRED_RESULT int desktop() const;
    void setIsHidden(bool b);
    Q_REQUIRED_RESULT bool isHidden() const;
    void setPosition(const QPoint &pos);
    Q_REQUIRED_RESULT QPoint position() const;
    void setShowInTaskbar(bool b);
    Q_REQUIRED_RESULT bool showInTaskbar() const;
    void setKeepAbove(bool b);
    Q_REQUIRED_RESULT bool keepAbove() const;
    void setKeepBelow(bool b);
    Q_REQUIRED_RESULT bool keepBelow() const;

    Q_REQUIRED_RESULT bool autoIndent() const;
    void setAutoIndent(bool b);
    Q_REQUIRED_RESULT bool operator==(const NoteDisplayAttribute &other) const;

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
    // it's an error but we can't remove it!
    bool mKeepBelove;
    bool mKeepBelow;
};
}
