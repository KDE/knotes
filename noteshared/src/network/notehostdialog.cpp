/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 2003, Daniel Martin <daniel.martin@pirack.com>
               2004, Michael Brade <brade@kde.org>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

 In addition, as a special exception, the copyright holders give
 permission to link the code of this program with any edition of
 the Qt library by Trolltech AS, Norway (or with modified versions
 of Qt that use the same license as Qt), and distribute linked
 combinations including the two.  You must obey the GNU General
 Public License in all respects for all of the code used other than
 Qt.  If you modify this file, you may extend this exception to
 your version of the file, but you are not obligated to do so.  If
 you do not wish to do so, delete this exception statement from
 your version.
*******************************************************************/

#include "notehostdialog.h"
#include "notesharedglobalconfig.h"

#include <kconfig.h>
#include "noteshared_debug.h"
#include <khistorycombobox.h>
#include <KLocalizedString>

#include <QVBoxLayout>
#include <dnssd/servicemodel.h>
#include <dnssd/servicebrowser.h>

#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QTreeView>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace NoteShared;
NoteHostDialog::NoteHostDialog(const QString &caption, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(caption);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setDefault(true);
    mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &NoteHostDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &NoteHostDialog::reject);



    QLabel *label = new QLabel(i18n("Select recipient:"), this);
    mainLayout->addWidget(label);

    m_servicesView = new QTreeView(this);
    m_servicesView->setRootIsDecorated(false);
    KDNSSD::ServiceModel *mdl = new KDNSSD::ServiceModel(new KDNSSD::ServiceBrowser(QStringLiteral("_knotes._tcp"), true), this);
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
    KDNSSD::RemoteService::Ptr srv = idx.data(KDNSSD::ServiceModel::ServicePtrRole).value<KDNSSD::RemoteService::Ptr>();
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
    KDNSSD::RemoteService::Ptr srv = idx.data(KDNSSD::ServiceModel::ServicePtrRole).value<KDNSSD::RemoteService::Ptr>();
    m_hostCombo->lineEdit()->setText(srv->hostName() + QLatin1String(":") + QString::number(srv->port()));
    accept();
}
