/*
  Copyright (c) 2013-2016 Montel Laurent <montel@kde.org>

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

#include "noteactionconfig.h"
#include "notesharedglobalconfig.h"

#include <QLineEdit>
#include <KLocalizedString>
#include <QDialog>

#include <QLabel>
#include <QVBoxLayout>
#include <QWhatsThis>
using namespace NoteShared;

NoteActionConfig::NoteActionConfig(QWidget *parent)
    : KCModule(parent)
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    QWidget *w =  new QWidget(this);
    lay->addWidget(w);
    QGridLayout *layout = new QGridLayout(w);
    layout->setMargin(0);

    QLabel *label_MailAction = new QLabel(i18n("&Mail action:"), this);
    layout->addWidget(label_MailAction, 0, 0);

    QLineEdit *kcfg_MailAction = new QLineEdit(this);
    kcfg_MailAction->setObjectName(QStringLiteral("kcfg_MailAction"));
    label_MailAction->setBuddy(kcfg_MailAction);
    layout->addWidget(kcfg_MailAction, 0, 1);

    QLabel *howItWorks = new QLabel(i18n("<a href=\"whatsthis\">How does this work?</a>"));
    connect(howItWorks, &QLabel::linkActivated, this, &NoteActionConfig::slotHelpLinkClicked);
    layout->addWidget(howItWorks, 1, 0);
    howItWorks->setContextMenuPolicy(Qt::NoContextMenu);

    addConfig(NoteShared::NoteSharedGlobalConfig::self(), w);
    lay->addStretch();
    load();
}

void NoteActionConfig::slotHelpLinkClicked(const QString &)
{
    const QString help =
        i18n("<qt>"
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
