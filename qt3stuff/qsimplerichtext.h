/****************************************************************************
** $Id$
**
** Definition of the QSimpleRichText class
**
** Created : 990101
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QSIMPLERICHTEXT_H
#define QSIMPLERICHTEXT_H

#ifndef QT_H
#include "qt3stuff.h"
#include "qstring.h"
#include "qregion.h"
#endif // QT_H


class QPainter;
class QWidget;
class QBrush;
class QMimeSourceFactory;

namespace Qt3 {

    class QStyleSheet;
    class QSimpleRichTextData;

class Q_EXPORT QSimpleRichText
{
public:
    QSimpleRichText( const QString& text, const QFont& fnt,
		     const QString& context = QString::null, const QStyleSheet* sheet = 0);
    QSimpleRichText( const QString& text, const QFont& fnt,
		     const QString& context,  const QStyleSheet* sheet,
		     const QMimeSourceFactory* factory, int verticalBreak = -1,
		     const QColor& linkColor = Qt::blue, bool linkUnderline = TRUE );
    ~QSimpleRichText();

    void setWidth( int );
    void setWidth( QPainter*, int );
    void setDefaultFont( const QFont &f );
    int width() const;
    int widthUsed() const;
    int height() const;
    void adjustSize();

    void draw( QPainter* p,  int x, int y, const QRect& clipRect,
	       const QColorGroup& cg, const QBrush* paper = 0) const;

    // obsolete
    void draw( QPainter* p,  int x, int y, const QRegion& clipRegion,
	       const QColorGroup& cg, const QBrush* paper = 0) const {
	draw( p, x, y, clipRegion.boundingRect(), cg, paper );
    }

    QString context() const;
    QString anchorAt( const QPoint& pos ) const;

    bool inText( const QPoint& pos ) const;

private:
    QSimpleRichTextData* d;
};

}; // namespace Qt3

#endif // QSIMPLERICHTEXT_H
