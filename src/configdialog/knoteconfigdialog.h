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

class KNoteCollectionConfigWidget;
class KNotePrintSelectThemeComboBox;

class QLineEdit;

class KNOTES_EXPORT KNoteConfigDialog : public KCMultiDialog
{
    Q_OBJECT
public:
    explicit KNoteConfigDialog(const QString &title, QWidget *parent);
    ~KNoteConfigDialog() override;

public Q_SLOTS:
    void slotOk();
};

class KNoteDisplayConfig : public KCModule
{
    Q_OBJECT
public:
    explicit KNoteDisplayConfig(QWidget *parent);
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
};

class KNoteEditorConfig : public KCModule
{
    Q_OBJECT
public:
    explicit KNoteEditorConfig(QWidget *parent);
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;
};

class KNoteMiscConfig : public KCModule
{
    Q_OBJECT
public:
    explicit KNoteMiscConfig(QWidget *parent);
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
    explicit KNotePrintConfig(QWidget *parent);
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
    explicit KNoteCollectionConfig(QWidget *parent);
    /** Reimplemented from KCModule. */
    void load() override;
    void save() override;

private:
    KNoteCollectionConfigWidget *mCollectionConfigWidget = nullptr;
};

#endif
