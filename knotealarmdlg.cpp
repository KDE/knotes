/*******************************************************************
 KNotes -- Notes for the KDE project

 Copyright (c) 2005, Michael Brade <brade@kde.org>

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

#include <qlabel.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qvbox.h>

#include <klocale.h>

#include <libkdepim/kdateedit.h>
#include <libkdepim/ktimeedit.h>

#include <libkcal/journal.h>
#include <libkcal/alarm.h>

#include "knotealarmdlg.h"


KNoteAlarmDlg::KNoteAlarmDlg( const QString& caption, QWidget *parent, const char *name )
    : KDialogBase( parent, name, true, caption, Ok|Cancel, Ok )
{
    QVBox *page = makeVBoxMainWidget();
    QGroupBox *group = new QGroupBox( 3, Vertical, i18n("Scheduled Alarm"), page );
    m_buttons = new QButtonGroup( page );
    m_buttons->hide();

    QRadioButton *none = new QRadioButton( i18n("&No alarm"), group );
    m_buttons->insert( none );

    QHBox *at = new QHBox( group );
    QRadioButton *label_at = new QRadioButton( i18n("Alarm &at:"), at );
    m_buttons->insert( label_at );
    m_atDate = new KDateEdit( at );
    m_atTime = new KTimeEdit( at );
    at->setStretchFactor( m_atDate, 1 );

    QHBox *in = new QHBox( group );
    QRadioButton *label_in = new QRadioButton( i18n("Alarm &in:"), in );
    m_buttons->insert( label_in );
    m_inTime = new KTimeEdit( in );
    QLabel *in_min = new QLabel( i18n("hours/minutes"), in );

    connect( m_buttons, SIGNAL(clicked( int )), SLOT(slotButtonChanged( int )) );
}


void KNoteAlarmDlg::setIncidence( KCal::Journal *journal )
{
    m_journal = journal;

    if ( !m_journal->alarms().isEmpty() )
    {
        KCal::Alarm *alarm = m_journal->alarms().first();
        if ( alarm->hasTime() )
        {
            m_buttons->setButton( 1 );
            m_atDate->setDate( alarm->time().date() );
            m_atTime->setTime( alarm->time().time() );
        }
        else if ( alarm->hasStartOffset() )
            m_buttons->setButton( 2 );
        else
            m_buttons->setButton( 0 );
    }
    else
        m_buttons->setButton( 0 );

    slotButtonChanged( m_buttons->selectedId() );
}

void KNoteAlarmDlg::slotButtonChanged( int id )
{
    switch ( id )
    {
    case 0:
        m_atDate->setEnabled( false );
        m_atTime->setEnabled( false );
        m_inTime->setEnabled( false );
        break;
    case 1:
        m_atDate->setEnabled( true );
        m_atTime->setEnabled( true );
        m_inTime->setEnabled( false );
        break;
    case 2:
        m_atDate->setEnabled( false );
        m_atTime->setEnabled( false );
        m_inTime->setEnabled( true );
    }
}

void KNoteAlarmDlg::slotOk()
{
    if ( m_buttons->selectedId() == 0 )
    {
        m_journal->clearAlarms();
        KDialogBase::slotOk();
        return;
    }

    KCal::Alarm *alarm;
    if ( m_journal->alarms().isEmpty() )
    {
        alarm = m_journal->newAlarm();
        alarm->setEnabled( true );
    }
    else
        alarm = m_journal->alarms().first();

    if ( m_buttons->selectedId() == 1 )
        alarm->setTime( QDateTime( m_atDate->date(), m_atTime->getTime() ) );
    else
    {
        // TODO
    }

    KDialogBase::slotOk();
}

#include "knotealarmdlg.moc"
