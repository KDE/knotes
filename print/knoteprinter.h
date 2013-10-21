#ifndef KNOTEPRINTER_H
#define KNOTEPRINTER_H

#include "knotes_export.h"

#include <grantlee/templateloader.h>

#include <QFont>

namespace Grantlee {
class Engine;
}

class QPrinter;
class KNotePrintObject;
class KNOTES_EXPORT KNotePrinter
{
public:
    KNotePrinter();
    ~KNotePrinter();

    void setDefaultFont( const QFont &font );
    QFont defaultFont() const;
    void printNotes(const QList<KNotePrintObject *> lst, const QString &themePath, bool preview);

private:
    void print(QPrinter &printer, const QString &htmlText);
    void doPrint( const QString &content, const QString &dialogCaption );
    void doPrintPreview(const QString &htmlText);
    
    QFont m_defaultFont;
    Grantlee::Engine *mEngine;
    Grantlee::FileSystemTemplateLoader::Ptr mTemplateLoader;
    Grantlee::Template mSelfcontainedTemplate;
};

#endif // KNOTEPRINTER
