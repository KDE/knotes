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

#include "notenetworkconfig.h"

#include "notesharedglobalconfig.h"

#include <QLineEdit>
#include <KLocalizedString>
#include <QDialog>
#include <QSpinBox>

#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QWhatsThis>
#include <QGroupBox>

using namespace NoteShared;
class NoteShared::NoteNetworkConfigWidgetPrivate
{
public:
    NoteNetworkConfigWidgetPrivate()
        : mTmpChkB(Q_NULLPTR),
          kcfg_SenderID(Q_NULLPTR),
          kcfg_Port(Q_NULLPTR)
    {

    }

    QCheckBox *mTmpChkB;
    QLineEdit *kcfg_SenderID;
    QSpinBox *kcfg_Port;
};

NoteNetworkConfigWidget::NoteNetworkConfigWidget(QWidget *parent)
    : QWidget(parent),
      d(new NoteShared::NoteNetworkConfigWidgetPrivate)
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    QWidget *w =  new QWidget(this);
    lay->addWidget(w);
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setMargin(0);

    QGroupBox *incoming = new QGroupBox(i18n("Incoming Notes"));
    QHBoxLayout *tmpLayout = new QHBoxLayout;

    d->mTmpChkB = new QCheckBox(i18n("Accept incoming notes"));
    d->mTmpChkB->setObjectName(QStringLiteral("kcfg_ReceiveNotes"));
    tmpLayout->addWidget(d->mTmpChkB);
    incoming->setLayout(tmpLayout);
    layout->addWidget(incoming);

    QGroupBox *outgoing = new QGroupBox(i18n("Outgoing Notes"));
    tmpLayout = new QHBoxLayout;

    QLabel *label_SenderID = new QLabel(i18n("&Sender ID:"));
    d->kcfg_SenderID = new QLineEdit;
    d->kcfg_SenderID->setClearButtonEnabled(true);
    d->kcfg_SenderID->setObjectName(QStringLiteral("d->kcfg_SenderID"));
    label_SenderID->setBuddy(d->kcfg_SenderID);
    tmpLayout->addWidget(label_SenderID);
    tmpLayout->addWidget(d->kcfg_SenderID);
    outgoing->setLayout(tmpLayout);
    layout->addWidget(outgoing);

    tmpLayout = new QHBoxLayout;

    QLabel *label_Port = new QLabel(i18n("&Port:"));

    tmpLayout->addWidget(label_Port);

    d->kcfg_Port = new QSpinBox;
    d->kcfg_Port->setObjectName(QStringLiteral("d->kcfg_Port"));
    d->kcfg_Port->setRange(0, 65535);
    label_Port->setBuddy(d->kcfg_Port);
    tmpLayout->addWidget(d->kcfg_Port);
    layout->addLayout(tmpLayout);
    lay->addStretch();
}

NoteNetworkConfigWidget::~NoteNetworkConfigWidget()
{
    delete d;
}

void NoteNetworkConfigWidget::save()
{
    NoteShared::NoteSharedGlobalConfig::self()->setReceiveNotes(d->mTmpChkB->isChecked());
    NoteShared::NoteSharedGlobalConfig::self()->setSenderID(d->kcfg_SenderID->text());
    NoteShared::NoteSharedGlobalConfig::self()->setPort(d->kcfg_Port->value());
    NoteShared::NoteSharedGlobalConfig::self()->save();
}

void NoteNetworkConfigWidget::load()
{
    d->mTmpChkB->setChecked(NoteShared::NoteSharedGlobalConfig::self()->receiveNotes());
    d->kcfg_SenderID->setText(NoteShared::NoteSharedGlobalConfig::self()->senderID());
    d->kcfg_Port->setValue(NoteShared::NoteSharedGlobalConfig::self()->port());
}

NoteNetworkConfig::NoteNetworkConfig(QWidget *parent)
    : KCModule(parent)
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setMargin(0);
    NoteNetworkConfigWidget *widget = new NoteNetworkConfigWidget(this);
    lay->addWidget(widget);
    addConfig(NoteShared::NoteSharedGlobalConfig::self(), widget);
    load();
}

void NoteNetworkConfig::save()
{
    KCModule::save();
}

void NoteNetworkConfig::load()
{
    KCModule::load();
}

