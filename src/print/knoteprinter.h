/*
   Copyright (C) 2013-2016 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KNOTEPRINTER_H
#define KNOTEPRINTER_H

#include "knotes_export.h"

#include <QFont>

class QPrinter;
class KNotePrintObject;
class KNoteGrantleePrint;
class KNOTES_EXPORT KNotePrinter : public QObject
{
    Q_OBJECT
public:
    explicit KNotePrinter(QObject *parent = Q_NULLPTR);
    ~KNotePrinter();

    void setDefaultFont(const QFont &font);
    QFont defaultFont() const;
    void printNotes(const QList<KNotePrintObject *> &lst, const QString &themePath, bool preview);

private Q_SLOTS:
    void slotPrinterPage(QPrinter *printer);
private:
    void print(QPrinter &printer, const QString &htmlText);
    void doPrint(const QString &content, const QString &dialogCaption);
    void doPrintPreview(const QString &htmlText);

    QFont m_defaultFont;
    QString mHtmlPreviewText;
    KNoteGrantleePrint *mGrantleePrint;

};

#endif // KNOTEPRINTER
