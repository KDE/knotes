/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "knotes_export.h"
#include <AkonadiCore/Item>
#include <QDialog>
class QTabWidget;
class KNoteEditorConfigWidget;
class KNoteDisplayConfigWidget;
class KNOTES_EXPORT KNoteSimpleConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KNoteSimpleConfigDialog(const QString &title, QWidget *parent = nullptr);
    ~KNoteSimpleConfigDialog();

    void load(Akonadi::Item &item, bool isRichText);
    void save(Akonadi::Item &item, bool &isRichText);

public Q_SLOTS:
    void slotUpdateCaption(const QString &name);

private:
    void readConfig();
    void writeConfig();
    QTabWidget *mTabWidget = nullptr;
    KNoteEditorConfigWidget *mEditorConfigWidget = nullptr;
    KNoteDisplayConfigWidget *mDisplayConfigWidget = nullptr;
};

