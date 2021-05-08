/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "notenetworkconfig.h"

#include "notesharedglobalconfig.h"

#include <KLocalizedString>
#include <QLineEdit>
#include <QSpinBox>

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWhatsThis>

using namespace NoteShared;
class NoteShared::NoteNetworkConfigWidgetPrivate
{
public:
    NoteNetworkConfigWidgetPrivate()
    {
    }

    QCheckBox *mTmpChkB = nullptr;
    QLineEdit *kcfg_SenderID = nullptr;
    QSpinBox *kcfg_Port = nullptr;
};

NoteNetworkConfigWidget::NoteNetworkConfigWidget(QWidget *parent)
    : QWidget(parent)
    , d(new NoteShared::NoteNetworkConfigWidgetPrivate)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto incoming = new QGroupBox(i18n("Incoming Notes"));
    auto tmpLayout = new QHBoxLayout;

    d->mTmpChkB = new QCheckBox(i18n("Accept incoming notes"));
    d->mTmpChkB->setObjectName(QStringLiteral("kcfg_ReceiveNotes"));
    tmpLayout->addWidget(d->mTmpChkB);
    incoming->setLayout(tmpLayout);
    layout->addWidget(incoming);

    auto outgoing = new QGroupBox(i18n("Outgoing Notes"));
    tmpLayout = new QHBoxLayout;

    auto label_SenderID = new QLabel(i18n("&Sender ID:"));
    d->kcfg_SenderID = new QLineEdit;
    d->kcfg_SenderID->setClearButtonEnabled(true);
    d->kcfg_SenderID->setObjectName(QStringLiteral("kcfg_SenderID"));
    label_SenderID->setBuddy(d->kcfg_SenderID);
    tmpLayout->addWidget(label_SenderID);
    tmpLayout->addWidget(d->kcfg_SenderID);
    outgoing->setLayout(tmpLayout);
    layout->addWidget(outgoing);

    tmpLayout = new QHBoxLayout;

    auto label_Port = new QLabel(i18n("&Port:"));

    tmpLayout->addWidget(label_Port);

    d->kcfg_Port = new QSpinBox;
    d->kcfg_Port->setObjectName(QStringLiteral("kcfg_Port"));
    d->kcfg_Port->setRange(0, 65535);
    label_Port->setBuddy(d->kcfg_Port);
    tmpLayout->addWidget(d->kcfg_Port);
    layout->addLayout(tmpLayout);
    layout->addStretch();
    load();
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

NoteNetworkConfig::NoteNetworkConfig(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    auto lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    auto noteNetworkConfigWidget = new NoteNetworkConfigWidget(this);
    lay->addWidget(noteNetworkConfigWidget);
    addConfig(NoteShared::NoteSharedGlobalConfig::self(), noteNetworkConfigWidget);
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
