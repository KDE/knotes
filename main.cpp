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

#include "knotesapp.h"
#include "version.h"
#include "main.h"


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
          I18N_NOOP("(c) 1997-2004, The KNotes Developers")
    );

    aboutData.addAuthor("Michael Brade", I18N_NOOP("Maintainer"), "brade@kde.org");
    aboutData.addAuthor("Bernd Johannes Wuebben", I18N_NOOP("Original KNotes Author"), "wuebben@kde.org");
    aboutData.addAuthor("Wynn Wilkes", I18N_NOOP("Ported KNotes to KDE 2"), "wynnw@calderasystems.com");
    aboutData.addAuthor("Bo Thorsen", I18N_NOOP("Started KDE Resource Framework Integration"), "bo@sonofthor.dk");

    aboutData.addCredit("Matthias Ettrich", 0, "ettrich@kde.org");
    aboutData.addCredit("David Faure", 0, "faure@kde.org");
    aboutData.addCredit("Matthias Kiefer", 0, "kiefer@kde.org");
    aboutData.addCredit("Laurent Montel", 0, "montel@kde.org");
    aboutData.addCredit("Dirk A. Mueller", 0, "dmuell@gmx.net");
    aboutData.addCredit("Carsten Pfeiffer", 0, "pfeiffer@kde.org");
    aboutData.addCredit("Harri Porten", 0, "porten@kde.org");
    aboutData.addCredit("Espen Sand", 0, "espen@kde.org");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KUniqueApplication::addCmdLineOptions();

    Application app;

    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    int rval = app.exec();

    return rval;
}
