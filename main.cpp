/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2003, The KNotes Developers

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*******************************************************************/

#include <kuniqueapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include <iostream>

#include "knotesapp.h"
#include "version.h"

using namespace std;

int main( int argc, char* argv[] )
{
    KAboutData aboutData( "knotes", I18N_NOOP("KNotes"),
        I18N_NOOP( KNOTES_VERSION ), I18N_NOOP( "KDE Notes" ), KAboutData::License_GPL,
        I18N_NOOP("(c) 1997-2003, The KNotes Developers") );

    aboutData.addAuthor("Michael Brade", I18N_NOOP("Maintainer"), "brade@kde.org");
    aboutData.addAuthor("Bernd Johannes Wuebben", I18N_NOOP("Original KNotes Author"), "wuebben@kde.org");
    aboutData.addAuthor("Wynn Wilkes", I18N_NOOP("Ported KNotes to KDE 2"), "wynnw@calderasystems.com");
    aboutData.addAuthor("Matthias Ettrich",0, "ettrich@kde.org");
    aboutData.addAuthor("Didier Belot",0, "dib@avo.fr");
    aboutData.addAuthor("Harri Porten",0, "porten@kde.org");
    aboutData.addAuthor("David Faure",0, "faure@kde.org");
    aboutData.addAuthor("Dirk A. Mueller",0, "dmuell@gmx.net");
    aboutData.addAuthor("Petter Reinholdtsen",0, "pere@td.org.uit.no");
    aboutData.addAuthor("Carsten Pfeiffer",0, "pfeiffer@kde.org");
    aboutData.addAuthor("Espen Sand",0, "espen@kde.org");
    aboutData.addAuthor("Matthias Kiefer",0, "kiefer@kde.org");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KUniqueApplication::addCmdLineOptions();

    // Check if unique application is already running...
    if ( !KUniqueApplication::start() )
    {
        cerr << "KNotes is already running, exiting..." << endl;
        return 1;
    }
    KUniqueApplication app;

    KNotesApp* a = new KNotesApp();

    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    a->show();

    int rval = app.exec();
    delete a;

    return rval;
}
