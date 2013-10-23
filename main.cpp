/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2013, The KNotes Developers

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*******************************************************************/

#include "kdepim-version.h"
#include "application.h"

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kxerrorhandler.h>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <QX11Info>
#endif

void remove_sm_from_client_leader();
KCmdLineOptions knotesOptions();
void knotesAuthors(  KAboutData &aboutData );

int main( int argc, char *argv[] )
{
    KAboutData aboutData( "knotes",
                          0,
                          ki18n( "KNotes" ),
                          KDEPIM_VERSION,
                          ki18n( "KDE Notes" ),
                          KAboutData::License_GPL,
                          ki18n( "Copyright © 1997–2013 KNotes authors" ) );

    knotesAuthors( aboutData );

    KCmdLineArgs::init( argc, argv, &aboutData );

    // Command line options

    KCmdLineArgs::addCmdLineOptions( knotesOptions() );

    KUniqueApplication::addCmdLineOptions();


    // Create Application

    Application app;
    KGlobal::locale()->insertCatalog(QLatin1String("libkdepim"));

    remove_sm_from_client_leader();

    return app.exec();
}

void remove_sm_from_client_leader()
{
#ifdef Q_WS_X11
    Atom type;
    int format, status;
    unsigned long nitems = 0;
    unsigned long extra = 0;
    unsigned char *data = 0;

    Atom atoms[ 2 ];
    char *atom_names[ 2 ] = { ( char * ) "WM_CLIENT_LEADER",
                              ( char * ) "SM_CLIENT_ID" };

    XInternAtoms( QX11Info::display(), atom_names, 2, False, atoms );

    QWidget w;
    KXErrorHandler handler; // ignore X errors
    status = XGetWindowProperty( QX11Info::display(), w.winId(), atoms[ 0 ], 0,
            10000, false, XA_WINDOW, &type, &format, &nitems,
            &extra, &data );

    if ( ( status == Success ) && !handler.error( false ) ) {
        if ( data && ( nitems > 0 ) ) {
            Window leader = * ( ( Window * ) data );
            XDeleteProperty( QX11Info::display(), leader, atoms[ 1 ] );
        }
        XFree( data );
    }
#endif
}

KCmdLineOptions knotesOptions()
{
    KCmdLineOptions options;

    options.add( "skip-note",
                 ki18n( "Suppress creation of a new note "
                        "on a non-unique instance." ) );

    return options;
}

void knotesAuthors(  KAboutData &aboutData )
{
    aboutData.addAuthor( ki18n( "Guillermo Antonio Amaral Bastidas" ),
                         ki18n( "Maintainer" ),
                         "me@guillermoamaral.com" );
    aboutData.addAuthor( ki18n( "Michael Brade" ),
                         ki18n( "Previous Maintainer" ),
                         "brade@kde.org" );
    aboutData.addAuthor( ki18n( "Bernd Johannes Wuebben" ),
                         ki18n( "Original KNotes Author" ),
                         "wuebben@kde.org" );
    aboutData.addAuthor( ki18n( "Wynn Wilkes" ),
                         ki18n( "Ported KNotes to KDE 2" ),
                         "wynnw@calderasystems.com" );
    aboutData.addAuthor( ki18n( "Daniel Martin" ),
                         ki18n( "Network Interface" ),
                         "daniel.martin@pirack.com" );
    aboutData.addAuthor( ki18n( "Bo Thorsen" ),
                         ki18n( "Started KDE Resource Framework Integration" ),
                         "bo@sonofthor.dk" );

    aboutData.addCredit( ki18n( "Bera Debajyoti" ),
                         ki18n( "Idea and initial code for the new look & feel" ),
                         "debajyotibera@gmail.com" );
    aboutData.addCredit( ki18n( "Matthias Ettrich" ),
                         KLocalizedString(),
                         "ettrich@kde.org" );
    aboutData.addCredit( ki18n( "David Faure" ),
                         KLocalizedString(),
                         "faure@kde.org" );
    aboutData.addCredit( ki18n( "Matthias Kiefer" ),
                         KLocalizedString(),
                         "kiefer@kde.org" );
    aboutData.addCredit( ki18n( "Luboš Luňák" ),
                         KLocalizedString(),
                         "l.lunak@kde.org" );
    aboutData.addCredit( ki18n( "Laurent Montel" ),
                         KLocalizedString(),
                         "montel@kde.org" );
    aboutData.addCredit( ki18n( "Dirk A. Mueller" ),
                         KLocalizedString(),
                         "dmuell@gmx.net" );
    aboutData.addCredit( ki18n( "Carsten Pfeiffer" ),
                         KLocalizedString(),
                         "pfeiffer@kde.org" );
    aboutData.addCredit( ki18n( "Harri Porten" ),
                         KLocalizedString(),
                         "porten@kde.org" );
    aboutData.addCredit( ki18n( "Espen Sand" ),
                         KLocalizedString(),
                         "espen@kde.org" );
}
