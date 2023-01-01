/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "notesagentnotedialog.h"
#include "attributes/notedisplayattribute.h"
#include "notesagent_debug.h"
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <KPIMTextEdit/RichTextEditor>
#include <KPIMTextEdit/RichTextEditorWidget>

#include <KSharedConfig>

#include <KMime/KMimeMessage>

#include <QIcon>

#include <KConfigGroup>
#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>

namespace
{
static const char myNotesAgentNoteDialogName[] = "NotesAgentNoteDialog";
}
NotesAgentNoteDialog::NotesAgentNoteDialog(QWidget *parent)
    : QDialog(parent)
    , mNote(new KPIMTextEdit::RichTextEditorWidget(this))
    , mSubject(new QLineEdit(this))
{
    auto mainLayout = new QVBoxLayout(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon::fromTheme(QStringLiteral("knotes")));

    mSubject->setReadOnly(true);
    mainLayout->addWidget(mSubject);

    mNote->setReadOnly(true);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &NotesAgentNoteDialog::reject);

    mainLayout->addWidget(mNote);
    mainLayout->addWidget(buttonBox);
    readConfig();
}

NotesAgentNoteDialog::~NotesAgentNoteDialog()
{
    writeConfig();
}

void NotesAgentNoteDialog::setNoteId(Akonadi::Item::Id id)
{
    Akonadi::Item item(id);
    auto job = new Akonadi::ItemFetchJob(item, this);
    job->fetchScope().fetchFullPayload(true);
    job->fetchScope().fetchAttribute<NoteShared::NoteDisplayAttribute>();
    connect(job, &Akonadi::ItemFetchJob::result, this, &NotesAgentNoteDialog::slotFetchItem);
}

void NotesAgentNoteDialog::slotFetchItem(KJob *job)
{
    if (job->error()) {
        qCDebug(NOTESAGENT_LOG) << "fetch item failed " << job->errorString();
        return;
    }
    auto itemFetchJob = static_cast<Akonadi::ItemFetchJob *>(job);
    const Akonadi::Item::List lstItem = itemFetchJob->items();
    if (!lstItem.isEmpty()) {
        const Akonadi::Item item = lstItem.first();
        auto noteMessage = item.payload<KMime::Message::Ptr>();
        if (noteMessage) {
            const KMime::Headers::Subject *const subject = noteMessage->subject(false);
            if (subject) {
                mSubject->setText(subject->asUnicodeString());
            }
            if (noteMessage->contentType()->isHTMLText()) {
                mNote->setAcceptRichText(true);
                mNote->setHtml(noteMessage->mainBodyPart()->decodedText());
            } else {
                mNote->setAcceptRichText(false);
                mNote->setPlainText(noteMessage->mainBodyPart()->decodedText());
            }
        }
        if (item.hasAttribute<NoteShared::NoteDisplayAttribute>()) {
            const auto attr = item.attribute<NoteShared::NoteDisplayAttribute>();
            if (attr) {
                mNote->editor()->setTextColor(attr->backgroundColor());
                // TODO add background color.
            }
        }
    }
}

void NotesAgentNoteDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(300, 200));
    KConfigGroup group(KSharedConfig::openStateConfig(), myNotesAgentNoteDialogName);
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void NotesAgentNoteDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myNotesAgentNoteDialogName);
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}
