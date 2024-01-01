/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knoteprinter.h"
#include "knotegrantleeprint.h"
#include "print/knoteprintobject.h"

#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QPointer>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextDocument>

#include <KMessageBox>
#include <KWindowStateSaver>
#include <QPrintPreviewDialog>

#include <KLocalizedString>

KNotePrinter::KNotePrinter(QObject *parent)
    : QObject(parent)
{
}

KNotePrinter::~KNotePrinter() = default;

void KNotePrinter::setDefaultFont(const QFont &font)
{
    m_defaultFont = font;
}

QFont KNotePrinter::defaultFont() const
{
    return m_defaultFont;
}

void KNotePrinter::doPrintPreview(const QString &htmlText)
{
    mHtmlPreviewText = htmlText;
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setCollateCopies(true);
    QPointer<QPrintPreviewDialog> previewdlg = new QPrintPreviewDialog(&printer);
    new KWindowStateSaver(previewdlg.data(), QStringLiteral("KNotePrintPreviewDialog"));
    connect(previewdlg.data(), &QPrintPreviewDialog::paintRequested, this, &KNotePrinter::slotPrinterPage);
    previewdlg->exec();
    delete previewdlg;
}

void KNotePrinter::slotPrinterPage(QPrinter *printer)
{
    print(*(printer), mHtmlPreviewText);
}

void KNotePrinter::doPrint(const QString &htmlText, const QString &dialogCaption)
{
    QPrinter printer(QPrinter::HighResolution);
    // printer.setFullPage( true );  //disabled, causes asymmetric margins
    QPointer<QPrintDialog> printDialog = new QPrintDialog(&printer);
    printDialog->setWindowTitle(dialogCaption);
    if (!printDialog->exec() || !printDialog) {
        delete printDialog;
        return;
    }
    print(printer, htmlText);
}

void KNotePrinter::print(QPrinter &printer, const QString &htmlText)
{
    const int margin = 30; // pt     //set to 40 when setFullPage() works again
    int marginX = margin * printer.logicalDpiX() / 72;
    int marginY = margin * printer.logicalDpiY() / 72;

    QRect typeArea(marginX, marginY, printer.width() - marginX * 2, printer.height() - marginY * 2);

    QTextDocument textDoc;
    textDoc.setHtml(htmlText);
    textDoc.documentLayout()->setPaintDevice(&printer);
    textDoc.setPageSize(typeArea.size());
    textDoc.setDefaultFont(m_defaultFont);

    QPainter painter(&printer);
    QRect clip(typeArea);
    painter.translate(marginX, marginY);
    clip.translate(-marginX, -marginY);

    for (int page = 1; page <= textDoc.pageCount(); ++page) {
        textDoc.drawContents(&painter, clip);
        clip.translate(0, typeArea.height());
        painter.translate(0, -typeArea.height());

        painter.setFont(m_defaultFont);
        const QString pageNumber(QString::number(page));
        painter.drawText(clip.right() - painter.fontMetrics().boundingRect(pageNumber).width(), clip.bottom() + painter.fontMetrics().ascent() + 5, pageNumber);

        if (page < textDoc.pageCount()) {
            printer.newPage();
        }
    }
}

void KNotePrinter::printNotes(const QList<KNotePrintObject *> &lst, const QString &themePath, bool preview)
{
    mGrantleePrint = std::make_unique<KNoteGrantleePrint>(themePath);
    if (mGrantleePrint->errorMessage().isEmpty()) {
        const QString htmlText = mGrantleePrint->notesToHtml(lst);
        const QString dialogCaption = i18np("Print Note", "Print %1 notes", lst.count());
        if (preview) {
            doPrintPreview(htmlText);
        } else {
            doPrint(htmlText, dialogCaption);
        }
    } else {
        KMessageBox::error(nullptr, i18n("Printing theme was not found."), i18n("Printing error"));
    }
}

#include "moc_knoteprinter.cpp"
