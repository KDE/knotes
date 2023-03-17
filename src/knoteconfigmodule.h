/*
   SPDX-FileCopyrightText: 2021-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "kcmutils_version.h"
#include <KCModule>
class QLineEdit;
class KNotePrintSelectThemeComboBox;
class KNoteCollectionConfigWidget;

class KNoteDisplayConfig : public KCModule
{
    Q_OBJECT
public:
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    explicit KNoteDisplayConfig(QWidget *parent, const QVariantList &args);
#else
    explicit KNoteDisplayConfig(QObject *parent, const KPluginMetaData &data, const QVariantList &args = QVariantList());
#endif
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
};

class KNoteEditorConfig : public KCModule
{
    Q_OBJECT
public:
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    explicit KNoteEditorConfig(QWidget *parent, const QVariantList &args);
#else
    explicit KNoteEditorConfig(QObject *parent, const KPluginMetaData &data, const QVariantList &args = QVariantList());
#endif
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
};

class KNoteMiscConfig : public KCModule
{
    Q_OBJECT
public:
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    explicit KNoteMiscConfig(QWidget *parent, const QVariantList &args);
#else
    explicit KNoteMiscConfig(QObject *parent, const KPluginMetaData &data, const QVariantList &args = QVariantList());
#endif
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
    void defaults() override;
private Q_SLOTS:
    void slotHelpLinkClicked(const QString &);

private:
    QLineEdit *mDefaultTitle = nullptr;
};

class KNotePrintConfig : public KCModule
{
    Q_OBJECT
public:
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    explicit KNotePrintConfig(QWidget *parent, const QVariantList &args);
#else
    explicit KNotePrintConfig(QObject *parent, const KPluginMetaData &data, const QVariantList &args = QVariantList());
#endif
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
    void defaults() override;

private Q_SLOTS:
    void slotThemeChanged();
    void slotDownloadNewThemes();

private:
    KNotePrintSelectThemeComboBox *mSelectTheme = nullptr;
};

class KNoteCollectionConfig : public KCModule
{
    Q_OBJECT
public:
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    explicit KNoteCollectionConfig(QWidget *parent, const QVariantList &args);
#else
    explicit KNoteCollectionConfig(QObject *parent, const KPluginMetaData &data, const QVariantList &args = QVariantList());
#endif
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;

private:
    KNoteCollectionConfigWidget *mCollectionConfigWidget = nullptr;
};
