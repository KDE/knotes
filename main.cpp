/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 1997-2004, The KNotes Developers

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
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*******************************************************************/

#include <kuniqueapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kxerrorhandler.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "knotesapp.h"
#include "version.h"
#include "main.h"


void remove_sm_from_client_leader()
{
    Atom type;
    int format, status;
    unsigned long nitems = 0;
    unsigned long extra = 0;
    unsigned char *data = 0;

    Atom atoms[ 2 ];
    char *atom_names[ 2 ] = { (char*)"WM_CLIENT_LEADER", (char*)"SM_CLIENT_ID" };

    XInternAtoms( qt_xdisplay(), atom_names, 2, False, atoms );

    QWidget w;
    KXErrorHandler handler; // ignore X errors
    status = XGetWindowProperty( qt_xdisplay(), w.winId(), atoms[ 0 ], 0, 10000,
                                 FALSE, XA_WINDOW, &type, &format,
                                 &nitems, &extra, &data );

    if (status  == Success && !handler.error( false ))
    {
        if (data && nitems > 0)
        {
            Window leader = *((Window*) data);
            XDeleteProperty( qt_xdisplay(), leader, atoms[ 1 ] );
        }
        XFree(data);
    }
}


Application::Application()
    : KUniqueApplication(), mMainWindow( 0 )
{
}

Application::~Application()
{
    delete mMainWindow;
}

int Application::newInstance()
{
    if ( !mMainWindow )
    {
        mMainWindow = new KNotesApp();
        mMainWindow->show();
    }
    else
        mMainWindow->newNote();

    return KUniqueApplication::newInstance();
}

int main( int argc, char* argv[] )
{
    QString version = QString::number( KNOTES_VERSION );

    KAboutData aboutData(
          "knotes",
          I18N_NOOP("KNotes"),
          version.latin1(),
          I18N_NOOP( "KDE Notes" ),
          KAboutData::License_GPL,
          I18N_NOOP("(c) 1997-2005, The KNotes Developers")
    );

    aboutData.addAuthor("Michael Brade", I18N_NOOP("Maintainer"), "brade@kde.org");
    aboutData.addAuthor("Bernd Johannes Wuebben", I18N_NOOP("Original KNotes Author"), "wuebben@kde.org");
    aboutData.addAuthor("Wynn Wilkes", I18N_NOOP("Ported KNotes to KDE 2"), "wynnw@calderasystems.com");
    aboutData.addAuthor("Daniel Martin", I18N_NOOP("Network Interface"), "daniel.martin@pirack.com");
    aboutData.addAuthor("Bo Thorsen", I18N_NOOP("Started KDE Resource Framework Integration"), "bo@sonofthor.dk");

    aboutData.addCredit("Matthias Ettrich", 0, "ettrich@kde.org");
    aboutData.addCredit("David Faure", 0, "faure@kde.org");
    aboutData.addCredit("Matthias Kiefer", 0, "kiefer@kde.org");
    aboutData.addCredit("Luboš Luňák", 0, "l.lunak@kde.org");
    aboutData.addCredit("Laurent Montel", 0, "montel@kde.org");
    aboutData.addCredit("Dirk A. Mueller", 0, "dmuell@gmx.net");
    aboutData.addCredit("Carsten Pfeiffer", 0, "pfeiffer@kde.org");
    aboutData.addCredit("Harri Porten", 0, "porten@kde.org");
    aboutData.addCredit("Espen Sand", 0, "espen@kde.org");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KUniqueApplication::addCmdLineOptions();

    Application app;
    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    remove_sm_from_client_leader();

    int rval = app.exec();

    return rval;
}
