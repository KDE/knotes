/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 2005 Michael Brade <brade@kde.org>

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#pragma once

#include "noteshared_export.h"
#include <QDialog>

class QDateTime;
class QAbstractButton;
class KDateComboBox;
class KTimeComboBox;
class QButtonGroup;
namespace NoteShared
{
class NoteAlarmDialogPrivate;
class NOTESHARED_EXPORT NoteAlarmDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NoteAlarmDialog(const QString &caption, QWidget *parent = nullptr);
    ~NoteAlarmDialog() override;
    void setAlarm(const QDateTime &dateTime);
    Q_REQUIRED_RESULT QDateTime alarm() const;

private:
    void slotButtonChanged(QAbstractButton *button);

private:
    KDateComboBox *m_atDate = nullptr;
    KTimeComboBox *m_atTime = nullptr;
    QButtonGroup *const m_buttons = nullptr;
};
}
