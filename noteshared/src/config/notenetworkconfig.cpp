/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

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

NoteNetworkConfigWidget::NoteNetworkConfigWidget(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    // layout->setContentsMargins(0, 0, 0, 0);

    auto incoming = new QGroupBox(i18n("Incoming Notes"));
    auto tmpLayout = new QHBoxLayout;

    mTmpChkB = new QCheckBox(i18n("Accept incoming notes"), this);
    mTmpChkB->setObjectName(QLatin1StringView("kcfg_ReceiveNotes"));
    tmpLayout->addWidget(mTmpChkB);
    incoming->setLayout(tmpLayout);
    layout->addWidget(incoming);

    auto outgoing = new QGroupBox(i18n("Outgoing Notes"), this);
    tmpLayout = new QHBoxLayout;

    auto label_SenderID = new QLabel(i18nc("@label:textbox", "&Sender ID:"), this);
    m_kcfg_SenderID = new QLineEdit;
    m_kcfg_SenderID->setClearButtonEnabled(true);
    m_kcfg_SenderID->setObjectName(QLatin1StringView("kcfg_SenderID"));
    label_SenderID->setBuddy(m_kcfg_SenderID);
    tmpLayout->addWidget(label_SenderID);
    tmpLayout->addWidget(m_kcfg_SenderID);
    outgoing->setLayout(tmpLayout);
    layout->addWidget(outgoing);

    tmpLayout = new QHBoxLayout;

    auto label_Port = new QLabel(i18nc("@label:textbox", "&Port:"), this);

    tmpLayout->addWidget(label_Port);

    kcfg_Port = new QSpinBox(this);
    kcfg_Port->setObjectName(QLatin1StringView("kcfg_Port"));
    kcfg_Port->setRange(0, 65535);
    label_Port->setBuddy(kcfg_Port);
    tmpLayout->addWidget(kcfg_Port);
    layout->addLayout(tmpLayout);
    layout->addStretch();
    load();
}

NoteNetworkConfigWidget::~NoteNetworkConfigWidget() = default;

void NoteNetworkConfigWidget::save()
{
    NoteShared::NoteSharedGlobalConfig::self()->setReceiveNotes(mTmpChkB->isChecked());
    NoteShared::NoteSharedGlobalConfig::self()->setSenderID(m_kcfg_SenderID->text());
    NoteShared::NoteSharedGlobalConfig::self()->setPort(kcfg_Port->value());
    NoteShared::NoteSharedGlobalConfig::self()->save();
}

void NoteNetworkConfigWidget::load()
{
    mTmpChkB->setChecked(NoteShared::NoteSharedGlobalConfig::self()->receiveNotes());
    m_kcfg_SenderID->setText(NoteShared::NoteSharedGlobalConfig::self()->senderID());
    kcfg_Port->setValue(NoteShared::NoteSharedGlobalConfig::self()->port());
}

NoteNetworkConfig::NoteNetworkConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    auto lay = new QVBoxLayout(widget());
    lay->setContentsMargins(0, 0, 0, 0);
    auto noteNetworkConfigWidget = new NoteNetworkConfigWidget(widget());
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

#include "moc_notenetworkconfig.cpp"
