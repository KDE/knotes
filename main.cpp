
#include "knotesapp.h"

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kuniqueapp.h>
#include <iostream.h>


int main( int argc, char* argv[] )
{
    KAboutData aboutData( "knotes", I18N_NOOP("KNotes"),
        I18N_NOOP("2.0pre"), I18N_NOOP( "KDE Notes" ), KAboutData::License_GPL,
        I18N_NOOP("(c) 1997-2000, KNote Developers") );

    aboutData.addAuthor("Bernd Johannes Wuebben",0, "wuebben@kde.org");
    aboutData.addAuthor("Matthias Ettrich",0, "ettrich@kde.org");
    aboutData.addAuthor("Didier Belot",0, "dib@avo.fr");
    aboutData.addAuthor("Harri Porten",0, "porten@kde.org");
    aboutData.addAuthor("David Faure",0, "faure@kde.org");
    aboutData.addAuthor("Dirk A. Mueller",0, "dmuell@gmx.net");
    aboutData.addAuthor("Petter Reinholdtsen",0, "pere@td.org.uit.no");
    aboutData.addAuthor("Carsten Pfeiffer",0, "pfeiffer@kde.org");
    aboutData.addAuthor("Wynn Wilkes",0, "wynnw@calderasystems.com");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KUniqueApplication::addCmdLineOptions();

    //check if unique application is already running...
    if( !KUniqueApplication::start() )
    {
        cerr << "already running, exiting..." << endl;
        return 1;
    }
    KUniqueApplication app;
    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    KNotesApp* a = new KNotesApp();
    a->show();

    int ret = app.exec();

    delete a;
    return ret;
}
