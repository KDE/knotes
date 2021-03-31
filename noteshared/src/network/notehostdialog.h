/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 2003 Daniel Martin <daniel.martin@pirack.com>
 SPDX-FileCopyrightText: 2004 Michael Brade <brade@kde.org>

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#pragma once

#include <QDialog>
class KHistoryComboBox;
class QTreeView;
class QModelIndex;
class QPushButton;

/**
 * A dialog that allows to select network service or request a hostname or IP address.
 */
namespace NoteShared
{
class NoteHostDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NoteHostDialog(const QString &caption, QWidget *parent = nullptr);
    ~NoteHostDialog() override;

    Q_REQUIRED_RESULT QString host() const;
    /**
     * Returns 0 if port was not specified
     */
    Q_REQUIRED_RESULT quint16 port() const;

private:
    void slotTextChanged(const QString &);
    void serviceSelected(const QModelIndex &);

    void slotServiceDoubleClicked(const QModelIndex &idx);
    void readConfig();
    KHistoryComboBox *m_hostCombo = nullptr;
    QTreeView *m_servicesView = nullptr;
    QPushButton *mOkButton = nullptr;
};
}

