/*
   SPDX-FileCopyrightText: 2013-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "noteshared_export.h"
#include <KCModule>

class QCheckBox;
class QLineEdit;
class QSpinBox;
namespace NoteShared
{
class NoteNetworkConfigWidgetPrivate;
class NOTESHARED_EXPORT NoteNetworkConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NoteNetworkConfigWidget(QWidget *parent = nullptr);
    ~NoteNetworkConfigWidget() override;

    void save();
    void load();

private:
    QCheckBox *mTmpChkB = nullptr;
    QLineEdit *m_kcfg_SenderID = nullptr;
    QSpinBox *kcfg_Port = nullptr;
};

class NOTESHARED_EXPORT NoteNetworkConfig : public KCModule
{
    Q_OBJECT
public:
    explicit NoteNetworkConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
};
}
