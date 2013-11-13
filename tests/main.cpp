/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

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
#include "knotesakonadiapp.h"
#include <kuniqueapplication.h>
#include <kstartupinfo.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <stdio.h>
#include <stdlib.h>

class KNotesApplication : public KUniqueApplication
{
public:
    /**
     * Similar to KUniqueApplication::newInstance, only without
     * the call to raise the widget when a second instance is started.
     */
    int newInstance() {
        return 0;
    }
};

int main( int argc, char *argv[] )
{
    KAboutData aboutData( "knotestray", 0,
                          ki18n( "knotestray" ),
                          "0.1",
                          ki18n( "test apps for knotes akonadi" ),
                          KAboutData::License_GPL,
                          ki18n( "(c) 2013 Laurent Montel <montel@kde.org>" ),
                          KLocalizedString() );
    aboutData.addAuthor( ki18n( "Laurent Montel" ), ki18n( "Maintainer and Author" ),
                         "montel@kde.org" );

    KCmdLineArgs::init( argc, argv, &aboutData );

    if ( !KUniqueApplication::start() ) {
        fprintf( stderr, "apps is already running!\n" );
        exit( 0 );
    }

    KNotesApplication a;
    a.setQuitOnLastWindowClosed( false );
    KNotesAkonadiApp app;
    app.show();
    return a.exec();
}
