/*
  This file is part of the KDE project

  Copyright (C) 2002 Daniel Molkentin <molkentin@kde.org>
  Copyright (c) 2004 Michael Brade <brade@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef KNOTES_PLUGIN_H
#define KNOTES_PLUGIN_H

#include <KontactInterface/Plugin>
#include <KontactInterface/UniqueAppHandler>
#include <KAboutData>

class KNotesUniqueAppHandler : public KontactInterface::UniqueAppHandler
{
    Q_OBJECT
public:
    explicit KNotesUniqueAppHandler(KontactInterface::Plugin *plugin)
        : KontactInterface::UniqueAppHandler(plugin)
    {
    }

    void loadCommandLineOptions(QCommandLineParser *parser) override;
    int activate(const QStringList &args, const QString &workingDir) override;
};

class KNotesPlugin : public KontactInterface::Plugin
{
    Q_OBJECT
public:
    KNotesPlugin(KontactInterface::Core *core, const QVariantList &);
    ~KNotesPlugin() override;

    KontactInterface::Summary *createSummaryWidget(QWidget *parentWidget) override;

    bool isRunningStandalone() const override;

    int weight() const override
    {
        return 600;
    }

    const KAboutData aboutData() override;

    bool canDecodeMimeData(const QMimeData *data) const override;
    void processDropEvent(QDropEvent *) override;
    void shortcutChanged() override;

protected:
    KParts::ReadOnlyPart *createPart() override;

private Q_SLOTS:
    void slotNewNote();

private:
    KontactInterface::UniqueAppWatcher *mUniqueAppWatcher = nullptr;
};

#endif
