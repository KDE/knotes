/*
  Copyright (c) 2013, 2014 Montel Laurent <montel@kde.org>

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

#ifndef KNOTEPRINTER_H
#define KNOTEPRINTER_H

#include "knotes_export.h"

#include <grantlee/templateloader.h>

#include <QFont>

namespace Grantlee
{
class Engine;
}

class QPrinter;
class KNotePrintObject;
class KNOTES_EXPORT KNotePrinter
{
public:
    KNotePrinter();
    ~KNotePrinter();

    void setDefaultFont(const QFont &font);
    QFont defaultFont() const;
    void printNotes(const QList<KNotePrintObject *> lst, const QString &themePath, bool preview);

private:
    void print(QPrinter &printer, const QString &htmlText);
    void doPrint(const QString &content, const QString &dialogCaption);
    void doPrintPreview(const QString &htmlText);

    QFont m_defaultFont;
    Grantlee::Engine *mEngine;
    QSharedPointer<Grantlee::FileSystemTemplateLoader> mTemplateLoader;
    Grantlee::Template mSelfcontainedTemplate;
};

#endif // KNOTEPRINTER
