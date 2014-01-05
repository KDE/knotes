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


#ifndef KNOTEEDITORCONFIGWIDGET_H
#define KNOTEEDITORCONFIGWIDGET_H

#include <QWidget>
class KIntNumInput;
class QCheckBox;
class KFontRequester;
namespace NoteShared {
class NoteDisplayAttribute;
}

class KNoteEditorConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KNoteEditorConfigWidget(bool defaults , QWidget *parent = 0);
    ~KNoteEditorConfigWidget();

    void load(NoteShared::NoteDisplayAttribute *attr, bool isRichText);
    void save(NoteShared::NoteDisplayAttribute *attr, bool &isRichText);

private:
    KIntNumInput *kcfg_TabSize;
    QCheckBox *kcfg_AutoIndent;
    QCheckBox *kcfg_RichText;
    KFontRequester *kcfg_Font;
    KFontRequester *kcfg_TitleFont;
};

#endif // KNOTEEDITORCONFIGWIDGET_H
