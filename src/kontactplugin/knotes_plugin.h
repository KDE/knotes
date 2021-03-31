/*
  This file is part of the KDE project

  SPDX-FileCopyrightText: 2002 Daniel Molkentin <molkentin@kde.org>
  SPDX-FileCopyrightText: 2004 Michael Brade <brade@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KAboutData>
#include <KontactInterface/Plugin>
#include <KontactInterface/UniqueAppHandler>

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
    KParts::Part *createPart() override;

private Q_SLOTS:
    void slotNewNote();

private:
    KontactInterface::UniqueAppWatcher *mUniqueAppWatcher = nullptr;
};

