/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <Akonadi/Attribute>

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

    [[nodiscard]] QByteArray type() const override;

    NoteDisplayAttribute *clone() const override;

    [[nodiscard]] QByteArray serialized() const override;

    void deserialize(const QByteArray &data) override;

    void setBackgroundColor(const QColor &color);
    [[nodiscard]] QColor backgroundColor() const;
    [[nodiscard]] QColor foregroundColor() const;
    void setForegroundColor(const QColor &color);

    [[nodiscard]] QSize size() const;
    void setSize(const QSize &size);

    [[nodiscard]] bool rememberDesktop() const;
    void setRememberDesktop(bool b);
    void setTabSize(int value);
    [[nodiscard]] int tabSize() const;

    void setFont(const QFont &f);
    [[nodiscard]] QFont font() const;
    void setTitleFont(const QFont &f);
    [[nodiscard]] QFont titleFont() const;

    void setDesktop(int v);
    [[nodiscard]] int desktop() const;
    void setIsHidden(bool b);
    [[nodiscard]] bool isHidden() const;
    void setPosition(const QPoint &pos);
    [[nodiscard]] QPoint position() const;
    void setShowInTaskbar(bool b);
    [[nodiscard]] bool showInTaskbar() const;
    void setKeepAbove(bool b);
    [[nodiscard]] bool keepAbove() const;
    void setKeepBelow(bool b);
    [[nodiscard]] bool keepBelow() const;

    [[nodiscard]] bool autoIndent() const;
    void setAutoIndent(bool b);
    [[nodiscard]] bool operator==(const NoteDisplayAttribute &other) const;

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
