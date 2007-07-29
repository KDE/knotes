#ifndef KNOTEPRINTER_H
#define KNOTEPRINTER_H

#include <QFont>
#include <QString>

template <class T> class QList;

namespace KCal {
class Journal;
}

class KNotePrinter {
public:

    KNotePrinter();

    void printNote( const QString &name,
                    const QString &htmlText ) const;

    void printNotes( const QList<KCal::Journal*>& journals ) const;

    void setDefaultFont( const QFont& font );
    QFont defaultFont() const;

private:
    void doPrint( const QString& content, const QString &dialogCaption ) const;
    inline QString ensureHtmlText( const QString &maybeRichText ) const;

    QFont m_defaultFont;
};

#endif // KNOTEPRINTER
