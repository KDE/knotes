/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knotedisplayconfigwidget.h"
#include <config-knotes.h>

#include "attributes/notedisplayattribute.h"
#include "attributes/notelockattribute.h"

#include <KColorButton>
#include <KLocalizedString>

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

KNoteDisplayConfigWidget::KNoteDisplayConfigWidget(bool defaults, QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QGridLayout(this);

    auto label_FgColor = new QLabel(i18n("&Text color:"), this);
    label_FgColor->setObjectName(QStringLiteral("label_FgColor"));
    layout->addWidget(label_FgColor, 0, 0);

    kcfg_FgColor = new KColorButton(this);
    kcfg_FgColor->setObjectName(QStringLiteral("kcfg_FgColor"));
    label_FgColor->setBuddy(kcfg_FgColor);
    layout->addWidget(kcfg_FgColor, 0, 1);

    auto label_BgColor = new QLabel(i18n("&Background color:"), this);
    label_BgColor->setObjectName(QStringLiteral("label_BgColor"));
    layout->addWidget(label_BgColor, 1, 0);

    kcfg_BgColor = new KColorButton(this);
    kcfg_BgColor->setObjectName(QStringLiteral("kcfg_BgColor"));
    label_BgColor->setBuddy(kcfg_BgColor);
    layout->addWidget(kcfg_BgColor, 1, 1);

#if KDEPIM_HAVE_X11
    kcfg_ShowInTaskbar = new QCheckBox(i18n("&Show note in taskbar"), this);
    kcfg_ShowInTaskbar->setObjectName(QStringLiteral("kcfg_ShowInTaskbar"));
    kcfg_RememberDesktop = new QCheckBox(i18n("&Remember desktop"), this);
    kcfg_RememberDesktop->setObjectName(QStringLiteral("kcfg_RememberDesktop"));
#endif
    if (defaults) {
        auto label_Width = new QLabel(i18n("Default &width:"), this);

        layout->addWidget(label_Width, 2, 0);

        kcfg_Width = new QSpinBox(this);
        kcfg_Width->setObjectName(QStringLiteral("kcfg_Width"));
        label_Width->setBuddy(kcfg_Width);
        kcfg_Width->setRange(50, 2000);
        kcfg_Width->setSingleStep(10);
        layout->addWidget(kcfg_Width, 2, 1);

        auto label_Height = new QLabel(i18n("Default &height:"), this);
        layout->addWidget(label_Height, 3, 0);

        kcfg_Height = new QSpinBox(this);
        kcfg_Height->setObjectName(QStringLiteral("kcfg_Height"));
        kcfg_Height->setRange(50, 2000);
        kcfg_Height->setSingleStep(10);
        label_Height->setBuddy(kcfg_Height);
        layout->addWidget(kcfg_Height, 3, 1);

#if KDEPIM_HAVE_X11
        layout->addWidget(kcfg_ShowInTaskbar, 4, 0);
        layout->addWidget(kcfg_RememberDesktop, 5, 0);
#endif
    } else {
#if KDEPIM_HAVE_X11
        layout->addWidget(kcfg_ShowInTaskbar, 2, 0);
        layout->addWidget(kcfg_RememberDesktop, 3, 0);
#endif
    }
    layout->setRowStretch(6, 1);
}

KNoteDisplayConfigWidget::~KNoteDisplayConfigWidget()
{
}

void KNoteDisplayConfigWidget::load(NoteShared::NoteDisplayAttribute *attr)
{
    if (attr) {
        kcfg_FgColor->setColor(attr->foregroundColor());
        kcfg_BgColor->setColor(attr->backgroundColor());
#if KDEPIM_HAVE_X11
        kcfg_ShowInTaskbar->setChecked(attr->showInTaskbar());
        kcfg_RememberDesktop->setChecked(attr->rememberDesktop());
#endif
        if (kcfg_Height) {
            kcfg_Height->setValue(attr->size().height());
        }
        if (kcfg_Width) {
            kcfg_Width->setValue(attr->size().width());
        }
    }
}

void KNoteDisplayConfigWidget::save(NoteShared::NoteDisplayAttribute *attr)
{
    if (attr) {
        attr->setForegroundColor(kcfg_FgColor->color());
        attr->setBackgroundColor(kcfg_BgColor->color());
#if KDEPIM_HAVE_X11
        attr->setShowInTaskbar(kcfg_ShowInTaskbar->isChecked());
        attr->setRememberDesktop(kcfg_RememberDesktop->isChecked());
#endif
        if (kcfg_Height && kcfg_Width) {
            attr->setSize(QSize(kcfg_Width->value(), kcfg_Height->value()));
        }
    }
}
