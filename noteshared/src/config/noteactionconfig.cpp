/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "noteactionconfig.h"
#include "notesharedglobalconfig.h"

#include <KLocalizedString>
#include <QLineEdit>

#include <QGridLayout>
#include <QLabel>
#include <QWhatsThis>
using namespace NoteShared;
NoteActionConfig::NoteActionConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    auto layout = new QGridLayout(widget());
    // layout->setContentsMargins(0, 0, 0, 0);

    auto label_MailAction = new QLabel(i18n("&Mail action:"), widget());
    layout->addWidget(label_MailAction, 0, 0);

    auto kcfg_MailAction = new QLineEdit(widget());
    kcfg_MailAction->setObjectName(QStringLiteral("kcfg_MailAction"));
    label_MailAction->setBuddy(kcfg_MailAction);
    layout->addWidget(kcfg_MailAction, 0, 1);

    auto howItWorks = new QLabel(i18n("<a href=\"whatsthis\">How does this work?</a>"));
    connect(howItWorks, &QLabel::linkActivated, this, &NoteActionConfig::slotHelpLinkClicked);
    layout->addWidget(howItWorks, 1, 0);
    howItWorks->setContextMenuPolicy(Qt::NoContextMenu);
    addConfig(NoteShared::NoteSharedGlobalConfig::self(), widget());
    layout->setRowStretch(2, 1);
    load();
}

void NoteActionConfig::slotHelpLinkClicked(const QString &)
{
    const QString help = i18n(
        "<qt>"
        "<p>You can customize command line. "
        "You can use:</p>"
        "<ul>"
        "<li>%t returns current note title</li>"
        "<li>%f returns current note text</li>"
        "</ul>"
        "</qt>");

    QWhatsThis::showText(QCursor::pos(), help);
}

void NoteActionConfig::save()
{
    KCModule::save();
}

void NoteActionConfig::load()
{
    KCModule::load();
}
