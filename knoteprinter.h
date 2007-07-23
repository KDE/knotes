#ifndef KNOTEPRINTER_H
#define KNOTEPRINTER_H

#include <qfont.h>
#include <qpalette.h>
#include <qstring.h>

class Q3MimeSourceFactory;
class Q3StyleSheet;
template <class T> class QList;
class KPrinter;

namespace KCal {
    class Journal;
}

class KNotePrinter {
public:

    KNotePrinter();

    void printNote( const QString& name,
                    const QString& content ) const;

    void printNotes( const QList<KCal::Journal*>& journals ) const;

    void setFont( const QFont& font );
    QFont font() const;

    void setColorGroup( const QColorGroup& colorGroup );
    QColorGroup colorGroup() const;

    void setStyleSheet( Q3StyleSheet* styleSheet );
    Q3StyleSheet* styleSheet() const;

    void setContext( const QString& context );
    QString context() const;

    void setMimeSourceFactory( Q3MimeSourceFactory* factory );
    Q3MimeSourceFactory* mimeSourceFactory() const;

private:
    void doPrint( KPrinter& printer, QPainter& painter,
                  const QString& content ) const;

    QColorGroup m_colorGroup;
    QFont m_font;
    Q3StyleSheet* m_styleSheet;
    Q3MimeSourceFactory* m_mimeSourceFactory;
    QString m_context;
};

#endif // KNOTEPRINTER
