/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "kcmutils_version.h"
#include "noteshared_export.h"
#include <KCModule>

#if KCMUTILS_VERSION >= QT_VERSION_CHECK(5, 240, 0)
#include <KPluginMetaData>
#endif
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
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    explicit NoteNetworkConfig(QWidget *parent, const QVariantList &args);
#else
    explicit NoteNetworkConfig(QObject *parent, const KPluginMetaData &data, const QVariantList &args = QVariantList());
#endif
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
};
}
