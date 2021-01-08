/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2005 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#ifndef KNOTECONFIGDIALOG_H
#define KNOTECONFIGDIALOG_H

#include "knotes_export.h"

#include <KCModule>
#include <KCMultiDialog>

class KNOTES_EXPORT KNoteConfigDialog : public KCMultiDialog
{
    Q_OBJECT
public:
    explicit KNoteConfigDialog(const QString &title, QWidget *parent);
    ~KNoteConfigDialog();

public Q_SLOTS:
    void slotOk();
};

#endif
