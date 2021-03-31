/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <AkonadiCore/Item>
#include <QString>
class QWidget;
namespace NoteShared
{
class NOTESHARED_EXPORT NoteUtils
{
public:
    struct NoteText {
        QString noteTitle;
        QString noteText;
    };
    NoteUtils();
    Q_REQUIRED_RESULT bool sendToMail(QWidget *parent, const QString &title, const QString &message);
    void sendToNetwork(QWidget *parent, const QString &title, const QString &message);
    Q_REQUIRED_RESULT QString createToolTip(const Akonadi::Item &item);
    Q_REQUIRED_RESULT NoteText extractNoteText(QString noteText, const QString &titleAddon);
};
}

