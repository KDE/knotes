/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "noteshared_export.h"
#include <Akonadi/Item>
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
    [[nodiscard]] bool sendToMail(QWidget *parent, const QString &title, const QString &message);
    void sendToNetwork(QWidget *parent, const QString &title, const QString &message);
    [[nodiscard]] QString createToolTip(const Akonadi::Item &item);
    [[nodiscard]] NoteText extractNoteText(QString noteText, const QString &titleAddon);
};
}
