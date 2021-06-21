/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 2003 Daniel Martin <daniel.martin@pirack.com>
 SPDX-FileCopyrightText: 2004 Michael Brade <brade@kde.org>

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include "notehostdialog.h"
#include "notesharedglobalconfig.h"

#include <kdnssd_version.h>
#if KDNSSD_VERSION >= QT_VERSION_CHECK(5, 84, 0)
#include <KDNSSD/ServiceBrowser>
#include <KDNSSD/ServiceModel>
#else
#include <DNSSD/ServiceBrowser>
#include <DNSSD/ServiceModel>
#endif
#include <KHistoryComboBox>
#include <KLocalizedString>

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

using namespace NoteShared;

NoteHostDialog::NoteHostDialog(const QString &caption, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(caption);

    auto mainLayout = new QVBoxLayout(this);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setDefault(true);
    mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &NoteHostDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &NoteHostDialog::reject);

    auto label = new QLabel(i18n("Select recipient:"), this);
    mainLayout->addWidget(label);

    m_servicesView = new QTreeView(this);
    m_servicesView->setRootIsDecorated(false);
    auto mdl = new KDNSSD::ServiceModel(new KDNSSD::ServiceBrowser(QStringLiteral("_knotes._tcp"), true), this);
    m_servicesView->setModel(mdl);
    m_servicesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_servicesView->hideColumn(KDNSSD::ServiceModel::Port);

    connect(m_servicesView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &NoteHostDialog::serviceSelected);
    connect(m_servicesView, &QTreeView::activated, this, &NoteHostDialog::serviceSelected);
    connect(m_servicesView, &QTreeView::clicked, this, &NoteHostDialog::serviceSelected);
    connect(m_servicesView, &QTreeView::doubleClicked, this, &NoteHostDialog::slotServiceDoubleClicked);

    mainLayout->addWidget(m_servicesView);

    label = new QLabel(i18n("Hostname or IP address:"), this);
    mainLayout->addWidget(label);

    m_hostCombo = new KHistoryComboBox(true, this);
    mainLayout->addWidget(m_hostCombo);
    m_hostCombo->setMinimumWidth(fontMetrics().maxWidth() * 15);
    m_hostCombo->setDuplicatesEnabled(false);

    // Read known hosts from configfile
    m_hostCombo->setHistoryItems(NoteShared::NoteSharedGlobalConfig::knownHosts(), true);
    m_hostCombo->setFocus();

    mainLayout->addWidget(buttonBox);

    connect(m_hostCombo->lineEdit(), &QLineEdit::textChanged, this, &NoteHostDialog::slotTextChanged);
    slotTextChanged(m_hostCombo->lineEdit()->text());
    readConfig();
}

NoteHostDialog::~NoteHostDialog()
{
    if (result() == Accepted) {
        m_hostCombo->addToHistory(m_hostCombo->currentText().trimmed());
    }

    // Write known hosts to configfile
    NoteShared::NoteSharedGlobalConfig::setKnownHosts(m_hostCombo->historyItems());
    NoteShared::NoteSharedGlobalConfig::setNoteHostDialogSize(size());
    NoteShared::NoteSharedGlobalConfig::setNoteHostDialogSize(size());
    NoteShared::NoteSharedGlobalConfig::self()->save();
}

void NoteHostDialog::readConfig()
{
    const QSize size = NoteShared::NoteSharedGlobalConfig::noteHostDialogSize();
    if (size.isValid()) {
        resize(size);
    }
}

void NoteHostDialog::slotTextChanged(const QString &text)
{
    mOkButton->setEnabled(!text.isEmpty());
}

void NoteHostDialog::serviceSelected(const QModelIndex &idx)
{
    auto srv = idx.data(KDNSSD::ServiceModel::ServicePtrRole).value<KDNSSD::RemoteService::Ptr>();
    m_hostCombo->lineEdit()->setText(srv->hostName() + QLatin1String(":") + QString::number(srv->port()));
}

QString NoteHostDialog::host() const
{
    return m_hostCombo->currentText().section(QLatin1Char(':'), 0, 0);
}

quint16 NoteHostDialog::port() const
{
    return m_hostCombo->currentText().section(QLatin1Char(':'), 1).toUShort();
}

void NoteHostDialog::slotServiceDoubleClicked(const QModelIndex &idx)
{
    auto srv = idx.data(KDNSSD::ServiceModel::ServicePtrRole).value<KDNSSD::RemoteService::Ptr>();
    m_hostCombo->lineEdit()->setText(srv->hostName() + QLatin1Char(':') + QString::number(srv->port()));
    accept();
}
