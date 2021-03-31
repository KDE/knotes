/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "knotes_export.h"

#include <QFont>

#include <memory>

class QPrinter;
class KNotePrintObject;
class KNoteGrantleePrint;
class KNOTES_EXPORT KNotePrinter : public QObject
{
    Q_OBJECT
public:
    explicit KNotePrinter(QObject *parent = nullptr);
    ~KNotePrinter() override;

    void setDefaultFont(const QFont &font);
    Q_REQUIRED_RESULT QFont defaultFont() const;
    void printNotes(const QList<KNotePrintObject *> &lst, const QString &themePath, bool preview);

private Q_SLOTS:
    void slotPrinterPage(QPrinter *printer);

private:
    void print(QPrinter &printer, const QString &htmlText);
    void doPrint(const QString &content, const QString &dialogCaption);
    void doPrintPreview(const QString &htmlText);

    QFont m_defaultFont;
    QString mHtmlPreviewText;
    std::unique_ptr<KNoteGrantleePrint> mGrantleePrint;
};

