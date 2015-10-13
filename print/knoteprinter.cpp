/*
  Copyright (c) 2013-2015 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "knoteprinter.h"
#include "print/knoteprintobject.h"
#include "knotegrantleeprint.h"

#include <QPainter>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>
#include <QAbstractTextDocumentLayout>
#include <QPointer>
#include <QPrintPreviewDialog>

#include <KMessageBox>

#include <KLocalizedString>
#include "knotes_debug.h"

#include <grantlee/context.h>
#include <grantlee/engine.h>
#include <grantlee/templateloader.h>

KNotePrinter::KNotePrinter(QObject *parent)
    : QObject(parent),
      mGrantleePrint(0)
{
}

KNotePrinter::~KNotePrinter()
{
}

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
    QPrintPreviewDialog previewdlg(&printer, 0);

    connect(&previewdlg, &QPrintPreviewDialog::paintRequested, this, &KNotePrinter::slotPrinterPage);
    previewdlg.exec();
}

void KNotePrinter::slotPrinterPage(QPrinter *printer)
{
    print(*(printer), mHtmlPreviewText);
}

void KNotePrinter::doPrint(const QString &htmlText,
                           const QString &dialogCaption)
{
    QPrinter printer(QPrinter::HighResolution);
    //printer.setFullPage( true );  //disabled, causes asymmetric margins
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
    const int margin = 30; //pt     //set to 40 when setFullPage() works again
    int marginX = margin * printer.logicalDpiX() / 72;
    int marginY = margin * printer.logicalDpiY() / 72;

    QRect typeArea(marginX, marginY,
                   printer.width() - marginX * 2,
                   printer.height() - marginY * 2);

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
        painter.drawText(
            clip.right() - painter.fontMetrics().width(pageNumber),
            clip.bottom() + painter.fontMetrics().ascent() + 5,
            pageNumber);

        if (page < textDoc.pageCount()) {
            printer.newPage();
        }
    }
}

void KNotePrinter::printNotes(const QList<KNotePrintObject *> lst, const QString &themePath, bool preview)
{
    mGrantleePrint = new KNoteGrantleePrint(themePath, this);
    if (mGrantleePrint->errorMessage().isEmpty()) {
        const QString htmlText = mGrantleePrint->notesToHtml(lst);
        const QString dialogCaption = i18np("Print Note", "Print %1 notes",
                                            lst.count());
        if (preview) {
            doPrintPreview(htmlText);
        } else {
            doPrint(htmlText, dialogCaption);
        }
    } else {
        KMessageBox::error(0, i18n("Printing theme was not found."), i18n("Printing error"));
    }
}

