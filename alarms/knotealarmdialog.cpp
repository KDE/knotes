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

#include "knotealarmdialog.h"

#include <kcal/alarm.h>
#include <kcal/journal.h>

#include <KDateComboBox>
#include <KLocale>
#include <KTimeComboBox>
#include <KVBox>

#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

KNoteAlarmDialog::KNoteAlarmDialog( const QString &caption, QWidget *parent )
    : KDialog( parent )
{
    setCaption( caption );
    setButtons( Ok | Cancel );
    KVBox *page = new KVBox( this );
    setMainWidget( page );

    m_buttons = new QButtonGroup( this );
    QGroupBox *group = new QGroupBox( i18n( "Scheduled Alarm" ), page );
    QVBoxLayout *layout = new QVBoxLayout;
    QRadioButton *none = new QRadioButton( i18n( "&No alarm" ) );
    layout->addWidget( none );
    m_buttons->addButton( none, 0 );

    group->setLayout( layout );

    KHBox *at = new KHBox;
    QRadioButton *label_at = new QRadioButton( i18n( "Alarm &at:" ), at );
    m_atDate = new KDateComboBox( at );
    m_atTime = new KTimeComboBox( at );
    at->setStretchFactor( m_atDate, 1 );
    layout->addWidget( at );
    m_buttons->addButton( label_at, 1 );

    KHBox *in = new KHBox;
    QRadioButton *label_in = new QRadioButton( i18n( "Alarm &in:" ), in );
    m_inTime = new KTimeComboBox( in );
    label_in->setEnabled( false ); // TODO
    //layout->addWidget( in );  //show it and enable it when feature will implemented
    m_buttons->addButton( label_in, 2 );

    connect( m_buttons, SIGNAL(buttonClicked(int)),
             SLOT(slotButtonChanged(int)) );
    connect( this, SIGNAL(okClicked()), SLOT(slotOk()) );
}


void KNoteAlarmDialog::setIncidence( KCal::Journal *journal )
{
    m_journal = journal;

    if ( !m_journal->alarms().isEmpty() ) {
        KCal::Alarm *alarm = m_journal->alarms().first();
        if ( alarm->hasTime() ) {
            m_buttons->button( 1 )->setChecked( true );
            m_atDate->setDate( alarm->time().date() );
            m_atTime->setTime( alarm->time().time() );
        } else if ( alarm->hasStartOffset() ) {
            m_buttons->button( 2 )->setChecked( true );
        } else {
            m_buttons->button( 0 )->setChecked( true );
        }
    } else {
        m_buttons->button( 0 )->setChecked( true );
        slotButtonChanged( m_buttons->checkedId() );
    }
}

void KNoteAlarmDialog::slotButtonChanged( int id )
{
    switch ( id ) {

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
        break;
    }
}

void KNoteAlarmDialog::slotOk()
{
    if ( m_buttons->checkedId() == 0 ) {
        m_journal->clearAlarms();
        return;
    }

    KCal::Alarm *alarm;
    if ( m_journal->alarms().isEmpty() ) {
        alarm = m_journal->newAlarm();
        alarm->setEnabled( true );
        alarm->setType( KCal::Alarm::Display );
    } else {
        alarm = m_journal->alarms().first();
    }

    if ( m_buttons->checkedId() == 1 ) {
        alarm->setTime( KDateTime( m_atDate->date(), m_atTime->time(),
                                   KDateTime::LocalZone ) );
    } else {
        // TODO
    }
}

#include "knotealarmdialog.moc"
