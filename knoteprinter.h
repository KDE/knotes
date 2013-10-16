#ifndef KNOTEPRINTER_H
#define KNOTEPRINTER_H

#include "knotes_export.h"
#include <QFont>
#include <QPrinter>

template <class T> class QList;

namespace KCal {
class Journal;
}


class KNOTES_EXPORT KNotePrinter
{
  public:
    KNotePrinter();
    

    void printNote(const QString &name,
                    const QString &htmlText , bool preview);
    
    void printNotes(const QList<KCal::Journal *> &journals , bool preview);
    
    void setDefaultFont( const QFont &font );
    QFont defaultFont() const;
    
  private:
    void print(QPrinter &printer, const QString &htmlText);
    void doPrint( const QString &content, const QString &dialogCaption );
    void doPrintPreview(const QString &htmlText);
    inline QString ensureHtmlText( const QString &maybeRichText ) const;
    
    QFont m_defaultFont;
};

#endif // KNOTEPRINTER
