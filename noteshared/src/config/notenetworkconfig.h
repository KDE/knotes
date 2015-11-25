/*
  Copyright (c) 2013-2015 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef NOTENETWORKCONFIG_H
#define NOTENETWORKCONFIG_H
#include "knotes/noteshared/src/noteshared_export.h"
#include <KCModule>
class QLineEdit;
class QCheckBox;
class QSpinBox;
namespace NoteShared
{
class NoteNetworkConfigWidgetPrivate;
class NOTESHARED_EXPORT NoteNetworkConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NoteNetworkConfigWidget(QWidget *parent = Q_NULLPTR);
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
    NoteNetworkConfig(QWidget *parent);
    /** Reimplemented from KCModule. */
    void load() Q_DECL_OVERRIDE;
    void save() Q_DECL_OVERRIDE;
};
}

#endif // NOTENETWORKCONFIG_H
