/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef NOTEACTIONCONFIG_H
#define NOTEACTIONCONFIG_H
#include "noteshared_export.h"
#include <KCModule>
namespace NoteShared
{
class NOTESHARED_EXPORT NoteActionConfig : public KCModule
{
    Q_OBJECT
public:
    explicit NoteActionConfig(QWidget *parent);
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;

private:
    void slotHelpLinkClicked(const QString &);
};
}
#endif // NOTEACTIONCONFIG_H
