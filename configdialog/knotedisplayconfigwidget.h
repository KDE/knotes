/*
  Copyright (c) 2013, 2014 Montel Laurent <montel@kde.org>

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

#ifndef KNOTEDISPLAYCONFIGWIDGET_H
#define KNOTEDISPLAYCONFIGWIDGET_H

#include <QWidget>
class KColorButton;
class QCheckBox;
class QSpinBox;
namespace NoteShared
{
class NoteDisplayAttribute;
}
class KNoteDisplayConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KNoteDisplayConfigWidget(bool defaults , QWidget *parent = 0);
    ~KNoteDisplayConfigWidget();

    void load(NoteShared::NoteDisplayAttribute *attr);
    void save(NoteShared::NoteDisplayAttribute *attr);

private:
    KColorButton *kcfg_FgColor;
    KColorButton *kcfg_BgColor;
    QCheckBox *kcfg_ShowInTaskbar;
    QCheckBox *kcfg_RememberDesktop;
    QSpinBox *kcfg_Width;
    QSpinBox *kcfg_Height;
};

#endif // KNOTEDISPLAYCONFIGWIDGET_H
