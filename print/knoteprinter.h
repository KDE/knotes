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

    void printNote(const QString &name,
                   const QString &htmlText , bool preview);
    
    void printNotes(const QList<KNotePrintObject *> lst, const QString &themePath, bool preview);
    void setDefaultFont( const QFont &font );
    QFont defaultFont() const;
    
private:
    void print(QPrinter &printer, const QString &htmlText);
    void doPrint( const QString &content, const QString &dialogCaption );
    void doPrintPreview(const QString &htmlText);
    inline QString ensureHtmlText( const QString &maybeRichText ) const;
    
    QFont m_defaultFont;
    Grantlee::Engine *mEngine;
    Grantlee::FileSystemTemplateLoader::Ptr mTemplateLoader;
    Grantlee::Template mSelfcontainedTemplate;
};

#endif // KNOTEPRINTER
