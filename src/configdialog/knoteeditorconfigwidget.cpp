/*
   Copyright (C) 2013-2016 Montel Laurent <montel@kde.org>

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
#include "knoteeditorconfigwidget.h"

#include "attributes/notelockattribute.h"

#include <QSpinBox>
#include <KLocalizedString>
#include <KFontRequester>

#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>

#include <attributes/notedisplayattribute.h>

KNoteEditorConfigWidget::KNoteEditorConfigWidget(bool defaults, QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    if (!defaults) {
        layout->setMargin(0);
    }

    QLabel *label_TabSize = new QLabel(i18n("&Tab size:"), this);
    layout->addWidget(label_TabSize, 0, 0, 1, 2);

    kcfg_TabSize = new QSpinBox(this);
    kcfg_TabSize->setObjectName(QStringLiteral("kcfg_TabSize"));
    kcfg_TabSize->setRange(1, 40);
    label_TabSize->setBuddy(kcfg_TabSize);
    layout->addWidget(kcfg_TabSize, 0, 2);

    kcfg_AutoIndent = new QCheckBox(i18n("Auto &indent"),
                                    this);
    kcfg_AutoIndent->setObjectName(QStringLiteral("kcfg_AutoIndent"));
    layout->addWidget(kcfg_AutoIndent, 1, 0, 1, 2);

    kcfg_RichText = new QCheckBox(i18n("&Rich text"), this);
    kcfg_RichText->setObjectName(QStringLiteral("kcfg_RichText"));
    layout->addWidget(kcfg_RichText, 1, 2);

    QLabel *label_Font = new QLabel(i18n("Text font:"), this);
    layout->addWidget(label_Font, 3, 0);

    kcfg_Font = new KFontRequester(this);
    kcfg_Font->setObjectName(QStringLiteral("kcfg_Font"));
    kcfg_Font->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                         QSizePolicy::Fixed));
    layout->addWidget(kcfg_Font, 3, 1, 1, 2);

    QLabel *label_TitleFont = new QLabel(i18n("Title font:"), this);
    layout->addWidget(label_TitleFont, 2, 0);

    kcfg_TitleFont = new KFontRequester(this);
    kcfg_TitleFont->setObjectName(QStringLiteral("kcfg_TitleFont"));
    kcfg_TitleFont->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                  QSizePolicy::Fixed));
    layout->addWidget(kcfg_TitleFont, 2, 1, 1, 2);
    layout->setRowStretch(4, 1);
}

KNoteEditorConfigWidget::~KNoteEditorConfigWidget()
{

}

void KNoteEditorConfigWidget::load(NoteShared::NoteDisplayAttribute *attr, bool isRichText)
{
    if (attr) {
        kcfg_TabSize->setValue(attr->tabSize());
        kcfg_AutoIndent->setChecked(attr->autoIndent());
        kcfg_RichText->setChecked(isRichText);
        kcfg_Font->setFont(attr->font());
        kcfg_TitleFont->setFont(attr->titleFont());
    }
}

void KNoteEditorConfigWidget::save(NoteShared::NoteDisplayAttribute *attr, bool &isRichText)
{
    if (attr) {
        attr->setTabSize(kcfg_TabSize->value());
        attr->setAutoIndent(kcfg_AutoIndent->isChecked());
        isRichText = kcfg_RichText->isChecked();
        attr->setFont(kcfg_Font->font());
        attr->setTitleFont(kcfg_TitleFont->font());
    }
}
