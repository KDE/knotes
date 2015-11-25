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

#include "notealarmdialog.h"

#include <KDateComboBox>
#include <KLocalizedString>
#include <KTimeComboBox>
#include <QVBoxLayout>
#include <QDateTime>

#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace NoteShared;

class NoteShared::NoteAlarmDialogPrivate
{
public:
    NoteAlarmDialogPrivate()
        : m_atDate(Q_NULLPTR),
          m_atTime(Q_NULLPTR),
          m_buttons(Q_NULLPTR)
    {

    }
    KDateComboBox *m_atDate;
    KTimeComboBox *m_atTime;
    QButtonGroup *m_buttons;

};

NoteAlarmDialog::NoteAlarmDialog(const QString &caption, QWidget *parent)
    : QDialog(parent),
      d(new NoteShared::NoteAlarmDialogPrivate)
{
    setWindowTitle(caption);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &NoteAlarmDialog::reject);
    QWidget *page = new QWidget(this);
    QVBoxLayout *pageVBoxLayout = new QVBoxLayout(page);
    pageVBoxLayout->setMargin(0);

    d->m_buttons = new QButtonGroup(this);
    QGroupBox *group = new QGroupBox(i18n("Scheduled Alarm"), page);
    pageVBoxLayout->addWidget(group);
    QVBoxLayout *layout = new QVBoxLayout;
    QRadioButton *none = new QRadioButton(i18n("&No alarm"));
    layout->addWidget(none);
    d->m_buttons->addButton(none, 0);

    group->setLayout(layout);

    QWidget *at = new QWidget;
    QHBoxLayout *atHBoxLayout = new QHBoxLayout(at);
    atHBoxLayout->setMargin(0);
    QRadioButton *label_at = new QRadioButton(i18n("Alarm &at:"), at);
    atHBoxLayout->addWidget(label_at);
    d->m_atDate = new KDateComboBox(at);
    atHBoxLayout->addWidget(d->m_atDate);
    d->m_atTime = new KTimeComboBox(at);
    atHBoxLayout->addWidget(d->m_atTime);
    const QDateTime dateTime = QDateTime::currentDateTime();
    d->m_atDate->setMinimumDate(dateTime.date());
    d->m_atTime->setMinimumTime(dateTime.time());
    atHBoxLayout->setStretchFactor(d->m_atDate, 1);
    layout->addWidget(at);
    d->m_buttons->addButton(label_at, 1);

    connect(d->m_buttons, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &NoteAlarmDialog::slotButtonChanged);
    connect(okButton, &QPushButton::clicked, this, &NoteAlarmDialog::accept);
    d->m_buttons->button(0)->setChecked(true);
    slotButtonChanged(d->m_buttons->checkedId());
    mainLayout->addWidget(page);
    mainLayout->addWidget(buttonBox);
}

NoteAlarmDialog::~NoteAlarmDialog()
{
    delete d;
}

void NoteAlarmDialog::setAlarm(const QDateTime &dateTime)
{
    if (dateTime.isValid()) {
        d->m_buttons->button(1)->setChecked(true);
        d->m_atDate->setDate(dateTime.date());
        d->m_atTime->setTime(dateTime.time());
    } else {
        d->m_buttons->button(0)->setChecked(true);
    }
    slotButtonChanged(d->m_buttons->checkedId());
}

void NoteAlarmDialog::slotButtonChanged(int id)
{
    switch (id) {
    case 0:
        d->m_atDate->setEnabled(false);
        d->m_atTime->setEnabled(false);
        break;
    case 1:
        d->m_atDate->setEnabled(true);
        d->m_atTime->setEnabled(true);
        break;
    }
}

QDateTime NoteAlarmDialog::alarm() const
{
    if (d->m_buttons->checkedId() == 1) {
        return QDateTime(d->m_atDate->date(), d->m_atTime->time());
    } else {
        return QDateTime();
    }
}
