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

#include "createnewnotejob.h"
#include "notesharedglobalconfig.h"
#include "attributes/showfoldernotesattribute.h"
#include "dialog/selectednotefolderdialog.h"
#include "noteshared_debug.h"
#include <Akonadi/Notes/NoteUtils>

#include <AkonadiCore/Collection>
#include <AkonadiCore/ItemCreateJob>
#include <AkonadiCore/EntityDisplayAttribute>
#include <AkonadiCore/Item>
#include <AkonadiCore/CollectionModifyJob>
#include <AkonadiCore/CollectionFetchJob>

#include <KMime/KMimeMessage>

#include <KMessageBox>
#include <QLocale>
#include <KLocalizedString>

#include <QPointer>

using namespace NoteShared;

class NoteShared::CreateNewNoteJobPrivate
{
public:
    CreateNewNoteJobPrivate(QWidget *widget)
        : mWidget(widget)
    {

    }
    QString mTitle;
    QString mText;
    bool mRichText;
    QWidget *mWidget;
};

CreateNewNoteJob::CreateNewNoteJob(QObject *parent, QWidget *widget)
    : QObject(parent),
      d(new NoteShared::CreateNewNoteJobPrivate(widget))
{
    connect(this, &CreateNewNoteJob::selectNewCollection, this, &CreateNewNoteJob::slotSelectNewCollection);
}

CreateNewNoteJob::~CreateNewNoteJob()
{
    delete d;
}

void CreateNewNoteJob::slotSelectNewCollection()
{
    createFetchCollectionJob(false);
}

void CreateNewNoteJob::setNote(const QString &name, const QString &text)
{
    d->mTitle = name;
    d->mText = text;
}

void CreateNewNoteJob::setRichText(bool richText)
{
    d->mRichText = richText;
}

void CreateNewNoteJob::start()
{
    createFetchCollectionJob(true);
}

void CreateNewNoteJob::createFetchCollectionJob(bool useSettings)
{
    Akonadi::Collection col;
    Akonadi::Collection::Id id = -1;
    if (useSettings) {
        id = NoteShared::NoteSharedGlobalConfig::self()->defaultFolder();
    } else {
        NoteShared::NoteSharedGlobalConfig::self()->setDefaultFolder(id);
    }
    if (id == -1) {
        QPointer<SelectedNotefolderDialog> dlg = new SelectedNotefolderDialog(d->mWidget);
        if (dlg->exec()) {
            col = dlg->selectedCollection();
        } else {
            deleteLater();
            return;
        }
        if (dlg->useFolderByDefault()) {
            NoteShared::NoteSharedGlobalConfig::self()->setDefaultFolder(col.id());
            NoteShared::NoteSharedGlobalConfig::self()->save();
        }
        delete dlg;
    } else {
        col = Akonadi::Collection(id);
    }
    NoteShared::NoteSharedGlobalConfig::self()->save();
    Akonadi::CollectionFetchJob *fetchCollection = new Akonadi::CollectionFetchJob(col, Akonadi::CollectionFetchJob::Base);
    connect(fetchCollection, &Akonadi::CollectionFetchJob::result, this, &CreateNewNoteJob::slotFetchCollection);
}

void CreateNewNoteJob::slotFetchCollection(KJob *job)
{
    if (job->error()) {
        qCDebug(NOTESHARED_LOG) << " Error during fetch: " << job->errorString();
        if (KMessageBox::Yes == KMessageBox::warningYesNo(0, i18n("An error occurred during fetching. Do you want select a new default collection?"))) {
            Q_EMIT selectNewCollection();
        } else {
            deleteLater();
        }
        return;
    }
    Akonadi::CollectionFetchJob *fetchCollection = qobject_cast<Akonadi::CollectionFetchJob *>(job);
    if (fetchCollection->collections().isEmpty()) {
        qCDebug(NOTESHARED_LOG) << "No collection fetched";
        if (KMessageBox::Yes == KMessageBox::warningYesNo(0, i18n("An error occurred during fetching. Do you want select a new default collection?"))) {
            Q_EMIT selectNewCollection();
        } else {
            deleteLater();
        }
        return;
    }
    Akonadi::Collection col = fetchCollection->collections().at(0);
    if (col.isValid()) {
        if (!col.hasAttribute<NoteShared::ShowFolderNotesAttribute>()) {
            if (KMessageBox::Yes == KMessageBox::warningYesNo(0, i18n("Collection is hidden. New note will stored but not displaying. Do you want to show collection?"))) {
                col.addAttribute(new NoteShared::ShowFolderNotesAttribute());
                Akonadi::CollectionModifyJob *job = new Akonadi::CollectionModifyJob(col);
                connect(job, &Akonadi::CollectionModifyJob::result, this, &CreateNewNoteJob::slotCollectionModifyFinished);
            }
        }
        Akonadi::Item newItem;
        newItem.setMimeType(Akonadi::NoteUtils::noteMimeType());

        KMime::Message::Ptr newPage = KMime::Message::Ptr(new KMime::Message());

        QString title;
        if (d->mTitle.isEmpty()) {
            const QDateTime currentDateTime = QDateTime::currentDateTime();
            title = NoteShared::NoteSharedGlobalConfig::self()->defaultTitle();
            title.replace(QStringLiteral("%t"), QLocale().toString(currentDateTime.time()));
            title.replace(QStringLiteral("%d"), QLocale().toString(currentDateTime.date(), QLocale::ShortFormat));
            title.replace(QStringLiteral("%l"), QLocale().toString(currentDateTime.date(), QLocale::LongFormat));
        } else {
            title = d->mTitle;
        }
        QByteArray encoding("utf-8");

        newPage->subject(true)->fromUnicodeString(title, encoding);
        newPage->contentType(true)->setMimeType(d->mRichText ? "text/html" : "text/plain");
        newPage->contentType()->setCharset("utf-8");
        newPage->contentTransferEncoding(true)->setEncoding(KMime::Headers::CEquPr);
        newPage->date(true)->setDateTime(QDateTime::currentDateTime());
        newPage->from(true)->fromUnicodeString(QStringLiteral("knotes@kde4"), encoding);
        // Need a non-empty body part so that the serializer regards this as a valid message.
        newPage->mainBodyPart()->fromUnicodeString(d->mText.isEmpty() ? QStringLiteral(" ") : d->mText);

        newPage->assemble();

        newItem.setPayload(newPage);

        Akonadi::EntityDisplayAttribute *eda = new Akonadi::EntityDisplayAttribute();

        eda->setIconName(QStringLiteral("text-plain"));
        newItem.addAttribute(eda);

        Akonadi::ItemCreateJob *job = new Akonadi::ItemCreateJob(newItem, col, this);
        connect(job, &Akonadi::ItemCreateJob::result, this, &CreateNewNoteJob::slotNoteCreationFinished);
    } else {
        deleteLater();
    }
}

void CreateNewNoteJob::slotNoteCreationFinished(KJob *job)
{
    if (job->error()) {
        qCWarning(NOTESHARED_LOG) << job->errorString();
        NoteShared::NoteSharedGlobalConfig::self()->setDefaultFolder(-1);
        NoteShared::NoteSharedGlobalConfig::self()->save();
        KMessageBox::error(d->mWidget, i18n("Note was not created."), i18n("Create new note"));
    }
    deleteLater();
}

void CreateNewNoteJob::slotCollectionModifyFinished(KJob *job)
{
    if (job->error()) {
        qCWarning(NOTESHARED_LOG) << job->errorString();
    }
}
