/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2015 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include "apps/application.h"
#include "knotes-version.h"
#include "knotes_debug.h"
#include "knotes_options.h"
#include <KLocalizedString>
#include <config-knotes.h>

#include <KAboutData>
#include <KCrash>

#include <QApplication>
#include <QCommandLineParser>

void knotesOptions(QCommandLineParser *parser);
void knotesAuthors(KAboutData &aboutData);

int main(int argc, char *argv[])
{
    // Disable session management
    qunsetenv("SESSION_MANAGER");

    Application app(argc, &argv);
    KCrash::initialize();

    KAboutData aboutData(QStringLiteral("knotes"),
                         i18n("KNotes"),
                         QStringLiteral(KNOTES_VERSION),
                         i18n("KDE Notes"),
                         KAboutLicense::GPL,
                         i18n("Copyright © 1997–%1 KNotes authors", QStringLiteral("2023")));
    knotesAuthors(aboutData);
    app.setAboutData(aboutData);
    QCommandLineParser *cmdArgs = app.cmdArgs();
    knotesOptions(cmdArgs);

    const QStringList args = QApplication::arguments();
    cmdArgs->process(args);
    aboutData.processCommandLine(cmdArgs);

    if (!Application::start(args)) {
        qCDebug(KNOTES_LOG) << " knotes already started";
        return 0;
    }

    return app.exec();
}

void knotesAuthors(KAboutData &aboutData)
{
    aboutData.addAuthor(i18nc("@info:credit", "Laurent Montel"), i18n("Maintainer"), QStringLiteral("montel@kde.org"));
    aboutData.addAuthor(i18nc("@info:credit", "Guillermo Antonio Amaral Bastidas"), i18n("Previous Maintainer"), QStringLiteral("me@guillermoamaral.com"));
    aboutData.addAuthor(i18nc("@info:credit", "Michael Brade"), i18n("Previous Maintainer"), QStringLiteral("brade@kde.org"));
    aboutData.addAuthor(i18nc("@info:credit", "Bernd Johannes Wuebben"), i18n("Original KNotes Author"), QStringLiteral("wuebben@kde.org"));
    aboutData.addAuthor(i18nc("@info:credit", "Wynn Wilkes"), i18n("Ported KNotes to KDE 2"), QStringLiteral("wynnw@calderasystems.com"));
    aboutData.addAuthor(i18nc("@info:credit", "Daniel Martin"), i18n("Network Interface"), QStringLiteral("daniel.martin@pirack.com"));
    aboutData.addAuthor(i18nc("@info:credit", "Bo Thorsen"), i18n("Started KDE Resource Framework Integration"), QStringLiteral("bo@sonofthor.dk"));
    aboutData.addCredit(i18nc("@info:credit", "Bera Debajyoti"),
                        i18n("Idea and initial code for the new look & feel"),
                        QStringLiteral("debajyotibera@gmail.com"));
    aboutData.addCredit(i18nc("@info:credit", "Matthias Ettrich"), QString(), QStringLiteral("ettrich@kde.org"));
    aboutData.addCredit(i18nc("@info:credit", "David Faure"), QString(), QStringLiteral("faure@kde.org"));
    aboutData.addCredit(i18nc("@info:credit", "Matthias Kiefer"), QString(), QStringLiteral("kiefer@kde.org"));
    aboutData.addCredit(i18nc("@info:credit", "Luboš Luňák"), QStringLiteral("l.lunak@kde.org"));
    aboutData.addCredit(i18nc("@info:credit", "Dirk A. Mueller"), QString(), QStringLiteral("dmuell@gmx.net"));
    aboutData.addCredit(i18nc("@info:credit", "Carsten Pfeiffer"), QString(), QStringLiteral("pfeiffer@kde.org"));
    aboutData.addCredit(i18nc("@info:credit", "Harri Porten"), QString(), QStringLiteral("porten@kde.org"));
    aboutData.addCredit(i18nc("@info:credit", "Espen Sand"), QString(), QStringLiteral("espen@kde.org"));
}
