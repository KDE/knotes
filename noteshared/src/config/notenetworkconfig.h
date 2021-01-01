/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef NOTENETWORKCONFIG_H
#define NOTENETWORKCONFIG_H
#include "noteshared_export.h"
#include <KCModule>
namespace NoteShared {
class NoteNetworkConfigWidgetPrivate;
class NOTESHARED_EXPORT NoteNetworkConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NoteNetworkConfigWidget(QWidget *parent = nullptr);
    ~NoteNetworkConfigWidget();

    void save();
    void load();
private:
    NoteNetworkConfigWidgetPrivate *const d;
};

class NOTESHARED_EXPORT NoteNetworkConfig : public KCModule
{
    Q_OBJECT
public:
    explicit NoteNetworkConfig(QWidget *parent);
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
};
}

#endif // NOTENETWORKCONFIG_H
