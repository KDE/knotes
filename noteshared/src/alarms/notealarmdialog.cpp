/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 2005 Michael Brade <brade@kde.org>

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include "notealarmdialog.h"

#include <KDateComboBox>
#include <KLocalizedString>
#include <KTimeComboBox>
#include <QDateTime>
#include <QVBoxLayout>

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>

using namespace NoteShared;


NoteAlarmDialog::NoteAlarmDialog(const QString &caption, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(caption);
    auto mainLayout = new QVBoxLayout(this);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &NoteAlarmDialog::reject);
    auto page = new QWidget(this);
    auto pageVBoxLayout = new QVBoxLayout(page);
    pageVBoxLayout->setContentsMargins(0, 0, 0, 0);

    m_buttons = new QButtonGroup(this);
    auto group = new QGroupBox(i18n("Scheduled Alarm"), page);
    pageVBoxLayout->addWidget(group);
    auto layout = new QVBoxLayout;
    auto none = new QRadioButton(i18n("&No alarm"));
    layout->addWidget(none);
    m_buttons->addButton(none, 0);

    group->setLayout(layout);

    auto at = new QWidget;
    auto atHBoxLayout = new QHBoxLayout(at);
    atHBoxLayout->setContentsMargins(0, 0, 0, 0);
    auto label_at = new QRadioButton(i18n("Alarm &at:"), at);
    atHBoxLayout->addWidget(label_at);
    m_atDate = new KDateComboBox(at);
    atHBoxLayout->addWidget(m_atDate);
    m_atTime = new KTimeComboBox(at);
    atHBoxLayout->addWidget(m_atTime);
    const QDateTime dateTime = QDateTime::currentDateTime();
    m_atDate->setMinimumDate(dateTime.date());
    m_atTime->setMinimumTime(dateTime.time());
    atHBoxLayout->setStretchFactor(m_atDate, 1);
    layout->addWidget(at);
    m_buttons->addButton(label_at, 1);

    connect(m_buttons, qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked), this, &NoteAlarmDialog::slotButtonChanged);
    connect(okButton, &QPushButton::clicked, this, &NoteAlarmDialog::accept);
    m_buttons->button(0)->setChecked(true);
    slotButtonChanged(m_buttons->checkedButton());
    mainLayout->addWidget(page);
    mainLayout->addWidget(buttonBox);
}

NoteAlarmDialog::~NoteAlarmDialog() = default;

void NoteAlarmDialog::setAlarm(const QDateTime &dateTime)
{
    if (dateTime.isValid()) {
        m_buttons->button(1)->setChecked(true);
        m_atDate->setDate(dateTime.date());
        m_atTime->setTime(dateTime.time());
    } else {
        m_buttons->button(0)->setChecked(true);
    }
    slotButtonChanged(m_buttons->checkedButton());
}

void NoteAlarmDialog::slotButtonChanged(QAbstractButton *button)
{
    if (button) {
        const int id = m_buttons->id(button);
        switch (id) {
        case 0:
            m_atDate->setEnabled(false);
            m_atTime->setEnabled(false);
            break;
        case 1:
            m_atDate->setEnabled(true);
            m_atTime->setEnabled(true);
            break;
        }
    }
}

QDateTime NoteAlarmDialog::alarm() const
{
    if (m_buttons->checkedId() == 1) {
        return QDateTime(m_atDate->date(), m_atTime->time());
    } else {
        return QDateTime();
    }
}
