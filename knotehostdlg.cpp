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
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

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

#include <qstring.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qvbox.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kcombobox.h>

#include "knotehostdlg.h"
#include "knotesglobalconfig.h"


KNoteHostDlg::KNoteHostDlg( const QString &caption, QWidget *parent, const char *name )
    : KDialogBase( parent, name, true, caption, Ok|Cancel, Ok, true )
{
    QVBox *page = makeVBoxMainWidget();
    (void)new QLabel( i18n("Hostname or IP address:"), page );

    m_hostCombo = new KHistoryCombo( true, page );
    m_hostCombo->setMinimumWidth( fontMetrics().maxWidth() * 15 );
    m_hostCombo->setDuplicatesEnabled( false );

    // Read known hosts from configfile
    m_hostCombo->setHistoryItems( KNotesGlobalConfig::knownHosts(), true );
    m_hostCombo->setFocus();
    //m_hostCombo->completionObject()->setItems( KNotesGlobalConfig::hostCompletions() );
    connect( m_hostCombo->lineEdit(), SIGNAL( textChanged ( const QString & ) ),
             this, SLOT( slotTextChanged( const QString & ) ) );
    slotTextChanged( m_hostCombo->lineEdit()->text() );
}

KNoteHostDlg::~KNoteHostDlg()
{
    if ( result() == Accepted )
        m_hostCombo->addToHistory( m_hostCombo->currentText().stripWhiteSpace() );

    // Write known hosts to configfile
    KNotesGlobalConfig::setKnownHosts( m_hostCombo->historyItems() );
    //KNotesGlobalConfig::setHostCompletions( m_hostCombo->completionObject()->items() );
    KNotesGlobalConfig::writeConfig();
}

void KNoteHostDlg::slotTextChanged( const QString& text )
{
    enableButtonOK( !text.isEmpty() );
}

QString KNoteHostDlg::host() const
{
    return m_hostCombo->currentText();
}


#include "knotehostdlg.moc"
