/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knotesgrantleeprinttest.h"
#include "knotegrantleeprint.h"
#include "print/knoteprintobject.h"

#include <Akonadi/Notes/NoteUtils>
#include <AkonadiCore/Item>

#include <KMime/Message>

#include <QTest>

KNotesGrantleePrintTest::KNotesGrantleePrintTest(QObject *parent)
    : QObject(parent)
{
}

KNotesGrantleePrintTest::~KNotesGrantleePrintTest()
{
}

void KNotesGrantleePrintTest::shouldReturnEmptyStringWhenNotContentAndNoNotes()
{
    KNoteGrantleePrint grantleePrint;
    QList<KNotePrintObject *> lst;
    const QString result = grantleePrint.notesToHtml(lst);
    QVERIFY(result.isEmpty());
}

void KNotesGrantleePrintTest::shouldReturnEmptyStringWhenAddContentWithoutNotes()
{
    KNoteGrantleePrint grantleePrint;
    grantleePrint.setTemplateContent(QStringLiteral("foo"));
    QList<KNotePrintObject *> lst;
    const QString result = grantleePrint.notesToHtml(lst);
    QVERIFY(result.isEmpty());
}

void KNotesGrantleePrintTest::shouldDisplayNoteInfo_data()
{
    QTest::addColumn<QString>("variable");
    QTest::addColumn<QString>("result");
    QTest::newRow("name") << QStringLiteral("name") << QStringLiteral("Test Note");
    QTest::newRow("description") << QStringLiteral("description") << QStringLiteral("notes test");
}

void KNotesGrantleePrintTest::shouldDisplayNoteInfo()
{
    QFETCH(QString, variable);
    QFETCH(QString, result);
    Akonadi::Item note(42);

    KNoteGrantleePrint grantleePrint;
    grantleePrint.setTemplateContent(QStringLiteral("{% if notes %}{% for note in notes %}{{ note.%1 }}{% endfor %}{% endif %}").arg(variable));

    KMime::Message::Ptr msg(new KMime::Message);
    note.setMimeType(Akonadi::NoteUtils::noteMimeType());
    QString subject = QStringLiteral("Test Note");
    msg->subject(true)->fromUnicodeString(subject, "us-ascii");
    msg->contentType(true)->setMimeType("text/plain");
    msg->contentType()->setCharset("utf-8");
    msg->contentTransferEncoding(true)->setEncoding(KMime::Headers::CEquPr);
    msg->date(true)->setDateTime(QDateTime::currentDateTime());
    msg->mainBodyPart()->fromUnicodeString(QStringLiteral("notes test"));
    note.setPayload(msg);
    msg->assemble();

    QList<KNotePrintObject *> lst;
    auto obj = new KNotePrintObject(note);
    lst << obj;
    const QString html = grantleePrint.notesToHtml(lst);
    QCOMPARE(html, result);
    qDeleteAll(lst);
}

QTEST_GUILESS_MAIN(KNotesGrantleePrintTest)
