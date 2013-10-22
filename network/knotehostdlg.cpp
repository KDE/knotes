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

#include "knotehostdlg.h"
#include "knotesglobalconfig.h"

#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <khistorycombobox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kvbox.h>
#include <dnssd/servicemodel.h>
#include <dnssd/servicebrowser.h>

#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QHeaderView>


KNoteHostDlg::KNoteHostDlg( const QString &caption, QWidget *parent )
    : KDialog( parent )
{
    setCaption( caption );
    setButtons( Ok|Cancel );
    KVBox *page = new KVBox( this );
    setMainWidget( page );
    ( void ) new QLabel( i18n("Select recipient:"), page );

    m_servicesView = new QTableView( page );
    m_servicesView->setShowGrid( false );
    DNSSD::ServiceModel* mdl = new DNSSD::ServiceModel( new DNSSD::ServiceBrowser( QLatin1String("_knotes._tcp"), true ), this );
    m_servicesView->setModel( mdl );
    m_servicesView->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_servicesView->hideColumn( DNSSD::ServiceModel::Port );
    connect( m_servicesView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
             SLOT(serviceSelected(QModelIndex)) );
    connect( m_servicesView, SIGNAL(activated(QModelIndex)),
             SLOT(serviceSelected(QModelIndex)) );
    connect( m_servicesView, SIGNAL(clicked(QModelIndex)),
             SLOT(serviceSelected(QModelIndex)) );

    ( void ) new QLabel( i18n("Hostname or IP address:"), page );

    m_hostCombo = new KHistoryComboBox( true, page );
    m_hostCombo->setMinimumWidth( fontMetrics().maxWidth() * 15 );
    m_hostCombo->setDuplicatesEnabled( false );

    // Read known hosts from configfile
    m_hostCombo->setHistoryItems( KNotesGlobalConfig::knownHosts(), true );
    m_hostCombo->setFocus();
    connect( m_hostCombo->lineEdit(), SIGNAL(textChanged(QString)),
             this, SLOT(slotTextChanged(QString)) );
    slotTextChanged( m_hostCombo->lineEdit()->text() );
    readConfig();
}

KNoteHostDlg::~KNoteHostDlg()
{
    if ( result() == Accepted ) {
        m_hostCombo->addToHistory( m_hostCombo->currentText().trimmed() );
    }

    // Write known hosts to configfile
    KNotesGlobalConfig::setKnownHosts( m_hostCombo->historyItems() );
    KNotesGlobalConfig::setNoteHostDialogSize(size());
    KNotesGlobalConfig::self()->writeConfig();
}

void KNoteHostDlg::readConfig()
{
    const QSize size = KNotesGlobalConfig::noteHostDialogSize();
    if ( size.isValid() ) {
        resize( size );
    }
}

void KNoteHostDlg::slotTextChanged( const QString &text )
{
    enableButton( Ok, !text.isEmpty() );
}

void KNoteHostDlg::serviceSelected( const QModelIndex& idx )
{
    DNSSD::RemoteService::Ptr srv=idx.data( DNSSD::ServiceModel::ServicePtrRole ).value<DNSSD::RemoteService::Ptr>();
    m_hostCombo->lineEdit()->setText( srv->hostName() + QLatin1String(":") + QString::number( srv->port() ) );
}

QString KNoteHostDlg::host() const
{
    return m_hostCombo->currentText().section( QLatin1Char(':'), 0, 0 );
}

quint16 KNoteHostDlg::port() const
{
    return m_hostCombo->currentText().section( QLatin1Char(':'), 1 ).toUShort();
}

#include "knotehostdlg.moc"
