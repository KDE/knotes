/*
  Copyright (c) 2013, 2014 Montel Laurent <montel@kde.org>

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

#include <QPainter>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>
#include <QAbstractTextDocumentLayout>
#include <QPointer>

#include <kdeprintdialog.h>
#include <KMessageBox>

#include <KLocalizedString>
#include <qdebug.h>
#include <KPrintPreview>

#include <grantlee/context.h>
#include <grantlee/engine.h>
#include <grantlee/templateloader.h>

KNotePrinter::KNotePrinter()
    : mEngine(new Grantlee::Engine)
{
    mTemplateLoader =  QSharedPointer<Grantlee::FileSystemTemplateLoader>(new Grantlee::FileSystemTemplateLoader);
}

KNotePrinter::~KNotePrinter()
{
    mEngine->deleteLater();
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
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setCollateCopies(true);

    KPrintPreview previewdlg(&printer, 0);
    print(printer, htmlText);
    previewdlg.exec();
}

void KNotePrinter::doPrint(const QString &htmlText,
                           const QString &dialogCaption)
{
    QPrinter printer(QPrinter::HighResolution);
    //printer.setFullPage( true );  //disabled, causes asymmetric margins
    QPointer<QPrintDialog> printDialog = KdePrint::createPrintDialog(&printer);
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
    mTemplateLoader->setTemplateDirs(QStringList() << themePath);
    mEngine->addTemplateLoader(mTemplateLoader);

    mSelfcontainedTemplate = mEngine->loadByName(QLatin1String("theme.html"));
    QString mErrorMessage;
    if (mSelfcontainedTemplate->error()) {
        mErrorMessage += mSelfcontainedTemplate->errorString() + QLatin1String("<br>");
    }

    if (mErrorMessage.isEmpty()) {
        QVariantList notes;
        Q_FOREACH (KNotePrintObject *n, lst) {
            notes << QVariant::fromValue(static_cast<QObject *>(n));
        }
        Grantlee::Context c;
        c.insert(QLatin1String("notes"), notes);
        c.insert(QLatin1String("alarm_i18n"), i18n("Alarm:"));
        c.insert(QLatin1String("note_is_locked_i18n"), i18n("Note is locked"));

        const QString htmlText = mSelfcontainedTemplate->render(&c);
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

