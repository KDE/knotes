#ifndef QCLEANUPHANDLER_H
#define QCLEANUPHANDLER_H

#ifndef QT_H
#include <qlist.h>
#endif // QT_H

#define QPtrList QList
#define QPtrListIterator QListIterator

namespace Qt3 {

template<class Type>
class Q_EXPORT QCleanupHandler
{
public:
    QCleanupHandler() : cleanupObjects( 0 ) {}
    ~QCleanupHandler() { clear(); }

    Type* add( Type **object ) {
        if ( !cleanupObjects )
            cleanupObjects = new QPtrList<Type*>;
        cleanupObjects->insert( 0, object );
        return *object;
    }

    void remove( Type **object ) {
        if ( !cleanupObjects )
            return;
        if ( cleanupObjects->findRef( object ) >= 0 )
            (void) cleanupObjects->take();
    }

    bool isEmpty() const {
        return cleanupObjects ? cleanupObjects->isEmpty() : TRUE;
    }

    void clear() {
        if ( !cleanupObjects )
            return;
        QPtrListIterator<Type*> it( *cleanupObjects );
        Type **object;
        while ( ( object = it.current() ) ) {
            delete *object;
            *object = 0;
            cleanupObjects->remove( object );
        }
        delete cleanupObjects;
        cleanupObjects = 0;
    }

private:
    QPtrList<Type*> *cleanupObjects;
};

}; // namespace Qt3

#endif //QCLEANUPHANDLER_H
