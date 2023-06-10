/*
   SPDX-FileCopyrightText: 2021-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KCModule>
class QLineEdit;
class KNotePrintSelectThemeComboBox;
class KNoteCollectionConfigWidget;

class KNoteDisplayConfig : public KCModule
{
    Q_OBJECT
public:
    explicit KNoteDisplayConfig(QObject *parent, const KPluginMetaData &data);
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
};

class KNoteEditorConfig : public KCModule
{
    Q_OBJECT
public:
    explicit KNoteEditorConfig(QObject *parent, const KPluginMetaData &data);
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
};

class KNoteMiscConfig : public KCModule
{
    Q_OBJECT
public:
    explicit KNoteMiscConfig(QObject *parent, const KPluginMetaData &data);
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
    explicit KNotePrintConfig(QObject *parent, const KPluginMetaData &data);
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
    explicit KNoteCollectionConfig(QObject *parent, const KPluginMetaData &data);
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;

private:
    KNoteCollectionConfigWidget *mCollectionConfigWidget = nullptr;
};
