/*
   SPDX-FileCopyrightText: 2013-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "createnewnotejob.h"
#include "attributes/showfoldernotesattribute.h"
#include "dialog/selectednotefolderdialog.h"
#include "noteshared_debug.h"
#include "notesharedglobalconfig.h"
#include <Akonadi/NoteUtils>

#include <Akonadi/Collection>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionModifyJob>
#include <Akonadi/EntityDisplayAttribute>
#include <Akonadi/Item>
#include <Akonadi/ItemCreateJob>

#include <KMime/KMimeMessage>

#include <KLocalizedString>
#include <KMessageBox>
#include <QLocale>

#include <QPointer>

using namespace NoteShared;

CreateNewNoteJob::CreateNewNoteJob(QObject *parent, QWidget *widget)
    : QObject(parent)
    , mWidget(widget)
{
    connect(this, &CreateNewNoteJob::selectNewCollection, this, &CreateNewNoteJob::slotSelectNewCollection);
}

CreateNewNoteJob::~CreateNewNoteJob() = default;

void CreateNewNoteJob::slotSelectNewCollection()
{
    createFetchCollectionJob(false);
}

void CreateNewNoteJob::setNote(const QString &name, const QString &text)
{
    mTitle = name;
    mText = text;
}

void CreateNewNoteJob::setRichText(bool richText)
{
    mRichText = richText;
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
        QPointer<SelectedNotefolderDialog> dlg = new SelectedNotefolderDialog(mWidget);
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
    auto fetchCollection = new Akonadi::CollectionFetchJob(col, Akonadi::CollectionFetchJob::Base);
    connect(fetchCollection, &Akonadi::CollectionFetchJob::result, this, &CreateNewNoteJob::slotFetchCollection);
}

void CreateNewNoteJob::slotFetchCollection(KJob *job)
{
    if (job->error()) {
        qCDebug(NOTESHARED_LOG) << " Error during fetch: " << job->errorString();
        const int answer = KMessageBox::warningYesNo(nullptr,
                                                     i18n("An error occurred during fetching. Do you want to select a new default collection?"),
                                                     QString(),
                                                     KGuiItem(i18nc("@action:button", "Select New Default")),
                                                     KGuiItem(i18nc("@action:button", "Ignore"), QStringLiteral("dialog-cancel")));
        if (answer == KMessageBox::Yes) {
            Q_EMIT selectNewCollection();
        } else {
            deleteLater();
        }
        return;
    }
    auto fetchCollection = qobject_cast<Akonadi::CollectionFetchJob *>(job);
    if (fetchCollection->collections().isEmpty()) {
        qCDebug(NOTESHARED_LOG) << "No collection fetched";
        const int answer = KMessageBox::warningYesNo(nullptr,
                                                     i18n("An error occurred during fetching. Do you want to select a new default collection?"),
                                                     QString(),
                                                     KGuiItem(i18nc("@action:button", "Select New Default")),
                                                     KGuiItem(i18nc("@action:button", "Ignore"), QStringLiteral("dialog-cancel")));
        if (answer == KMessageBox::Yes) {
            Q_EMIT selectNewCollection();
        } else {
            deleteLater();
        }
        return;
    }
    Akonadi::Collection col = fetchCollection->collections().at(0);
    if (col.isValid()) {
        if (!col.hasAttribute<NoteShared::ShowFolderNotesAttribute>()) {
            const int answer =
                KMessageBox::warningYesNo(nullptr,
                                          i18n("Collection is hidden. New note will be stored but not displayed. Do you want to show collection?"),
                                          QString(),
                                          KGuiItem(i18nc("@action::button", "Show Collection")),
                                          KGuiItem(i18nc("@action::button", "Do Not Show"), QStringLiteral("dialog-cancel")));
            if (answer == KMessageBox::Yes) {
                col.addAttribute(new NoteShared::ShowFolderNotesAttribute());
                auto modifyJob = new Akonadi::CollectionModifyJob(col);
                connect(modifyJob, &Akonadi::CollectionModifyJob::result, this, &CreateNewNoteJob::slotCollectionModifyFinished);
            }
        }
        Akonadi::Item newItem;
        newItem.setMimeType(Akonadi::NoteUtils::noteMimeType());

        KMime::Message::Ptr newPage = KMime::Message::Ptr(new KMime::Message());

        QString title;
        if (mTitle.isEmpty()) {
            const QDateTime currentDateTime = QDateTime::currentDateTime();
            title = NoteShared::NoteSharedGlobalConfig::self()->defaultTitle();
            title.replace(QStringLiteral("%t"), QLocale().toString(currentDateTime.time()));
            title.replace(QStringLiteral("%d"), QLocale().toString(currentDateTime.date(), QLocale::ShortFormat));
            title.replace(QStringLiteral("%l"), QLocale().toString(currentDateTime.date(), QLocale::LongFormat));
        } else {
            title = mTitle;
        }
        QByteArray encoding("utf-8");

        newPage->subject(true)->fromUnicodeString(title, encoding);
        newPage->contentType(true)->setMimeType(mRichText ? "text/html" : "text/plain");
        newPage->contentType()->setCharset("utf-8");
        newPage->contentTransferEncoding(true)->setEncoding(KMime::Headers::CEquPr);
        newPage->date(true)->setDateTime(QDateTime::currentDateTime());
        newPage->from(true)->fromUnicodeString(QStringLiteral("knotes@kde4"), encoding);
        // Need a non-empty body part so that the serializer regards this as a valid message.
        newPage->mainBodyPart()->fromUnicodeString(mText.isEmpty() ? QStringLiteral(" ") : mText);

        newPage->assemble();

        newItem.setPayload(newPage);

        auto eda = new Akonadi::EntityDisplayAttribute();

        eda->setIconName(QStringLiteral("text-plain"));
        newItem.addAttribute(eda);

        auto createJob = new Akonadi::ItemCreateJob(newItem, col, this);
        connect(createJob, &Akonadi::ItemCreateJob::result, this, &CreateNewNoteJob::slotNoteCreationFinished);
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
        KMessageBox::error(mWidget, i18n("Note was not created."), i18n("Create new note"));
    }
    deleteLater();
}

void CreateNewNoteJob::slotCollectionModifyFinished(KJob *job)
{
    if (job->error()) {
        qCWarning(NOTESHARED_LOG) << job->errorString();
    }
}
