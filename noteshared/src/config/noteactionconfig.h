/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "kcmutils_version.h"
#include "noteshared_export.h"
#include <KCModule>
#include <KPluginMetaData>
namespace NoteShared
{
class NOTESHARED_EXPORT NoteActionConfig : public KCModule
{
    Q_OBJECT
public:
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    explicit NoteActionConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());
#else
    explicit NoteActionConfig(QObject *parent, const KPluginMetaData &data, const QVariantList &args = QVariantList());
#endif
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;

private:
    void slotHelpLinkClicked(const QString &);
};
}
