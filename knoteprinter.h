#ifndef KNOTEPRINTER_H
#define KNOTEPRINTER_H

#include <qfont.h>
#include <qpalette.h>
#include <qstring.h>

class QMimeSourceFactory;
class QStyleSheet;
template <class T> class QValueList;
class KPrinter;

namespace KCal {
    class Journal;
}

class KNotePrinter {
public:

    KNotePrinter();

    void printNote( const QString& name,
                    const QString& content ) const;

    void printNotes( const QValueList<KCal::Journal*>& journals ) const;

    void setFont( const QFont& font );
    QFont font() const;

    void setColorGroup( const QColorGroup& colorGroup );
    QColorGroup colorGroup() const;

    void setStyleSheet( QStyleSheet* styleSheet );
    QStyleSheet* styleSheet() const;

    void setContext( const QString& context );
    QString context() const;

    void setMimeSourceFactory( QMimeSourceFactory* factory );
    QMimeSourceFactory* mimeSourceFactory() const;

private:
    void doPrint( KPrinter& printer, QPainter& painter,
                  const QString& content ) const;

    QColorGroup m_colorGroup;
    QFont m_font;
    QStyleSheet* m_styleSheet;
    QMimeSourceFactory* m_mimeSourceFactory;
    QString m_context;
};

#endif // KNOTEPRINTER
