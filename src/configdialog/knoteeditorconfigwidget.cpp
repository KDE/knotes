/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knoteeditorconfigwidget.h"
#include "attributes/notedisplayattribute.h"

#include <KLocalizedString>

#include <KFontRequester>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

KNoteEditorConfigWidget::KNoteEditorConfigWidget(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QGridLayout(this);

    auto label_TabSize = new QLabel(i18n("&Tab size:"), this);
    layout->addWidget(label_TabSize, 0, 0, 1, 2);

    kcfg_TabSize = new QSpinBox(this);
    kcfg_TabSize->setObjectName(QStringLiteral("kcfg_TabSize"));
    kcfg_TabSize->setRange(1, 40);
    label_TabSize->setBuddy(kcfg_TabSize);
    layout->addWidget(kcfg_TabSize, 0, 2);

    kcfg_AutoIndent = new QCheckBox(i18n("Auto &indent"), this);
    kcfg_AutoIndent->setObjectName(QStringLiteral("kcfg_AutoIndent"));
    layout->addWidget(kcfg_AutoIndent, 1, 0, 1, 2);

    kcfg_RichText = new QCheckBox(i18n("&Rich text"), this);
    kcfg_RichText->setObjectName(QStringLiteral("kcfg_RichText"));
    layout->addWidget(kcfg_RichText, 1, 2);

    auto label_Font = new QLabel(i18n("Text font:"), this);
    layout->addWidget(label_Font, 3, 0);

    kcfg_Font = new KFontRequester(this);
    kcfg_Font->setObjectName(QStringLiteral("kcfg_Font"));
    kcfg_Font->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
    layout->addWidget(kcfg_Font, 3, 1, 1, 2);

    auto label_TitleFont = new QLabel(i18n("Title font:"), this);
    layout->addWidget(label_TitleFont, 2, 0);

    kcfg_TitleFont = new KFontRequester(this);
    kcfg_TitleFont->setObjectName(QStringLiteral("kcfg_TitleFont"));
    kcfg_TitleFont->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
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
