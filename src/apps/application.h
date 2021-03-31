/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2013 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#pragma once

#include <KontactInterface/PimUniqueApplication>

class KNotesApp;

class Application : public KontactInterface::PimUniqueApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv[]);
    ~Application() override;

    int activate(const QStringList &args, const QString &workingDir) override;

private:
    KNotesApp *mMainWindow = nullptr;
};

