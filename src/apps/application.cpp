/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2013 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include "application.h"
#include "apps/knotesapp.h"

#include <QCommandLineParser>

Application::Application(int &argc, char **argv[])
    : KontactInterface::PimUniqueApplication(argc, argv)
{
}

Application::~Application()
{
    delete mMainWindow;
}

int Application::activate(const QStringList &args, const QString &workingDir)
{
    Q_UNUSED(workingDir)

    QCommandLineParser *parser = cmdArgs();
    parser->process(args);

    if (!mMainWindow) {
        mMainWindow = new KNotesApp();
    } else {
        if (!parser->isSet(QStringLiteral("skip-note"))) {
            mMainWindow->newNote();
        }
    }

    return 0;
}
