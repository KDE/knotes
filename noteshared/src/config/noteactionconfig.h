/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "noteshared_export.h"
#include <KCModule>
#include <KPluginMetaData>
namespace NoteShared
{
class NOTESHARED_EXPORT NoteActionConfig : public KCModule
{
    Q_OBJECT
public:
    explicit NoteActionConfig(QObject *parent, const KPluginMetaData &data);
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;

private:
    void slotHelpLinkClicked(const QString &);
};
}
