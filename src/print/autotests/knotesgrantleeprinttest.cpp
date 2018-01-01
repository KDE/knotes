/*
   Copyright (C) 2015-2018 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "knotesgrantleeprinttest.h"
#include "knotegrantleeprint.h"
#include "print/knoteprintobject.h"

#include <AkonadiCore/Item>
#include <Akonadi/Notes/NoteUtils>

#include <KMime/Message>

#include <QTest>

KNotesGrantleePrintTest::KNotesGrantleePrintTest(QObject *parent)
    : QObject(parent)
{
}

KNotesGrantleePrintTest::~KNotesGrantleePrintTest()
{
}

void KNotesGrantleePrintTest::shouldDefaultValue()
{
    KNoteGrantleePrint *grantleePrint = new KNoteGrantleePrint;
    QVERIFY(grantleePrint);
    grantleePrint->deleteLater();
    grantleePrint = nullptr;
}

void KNotesGrantleePrintTest::shouldReturnEmptyStringWhenNotContentAndNoNotes()
{
    KNoteGrantleePrint *grantleePrint = new KNoteGrantleePrint;
    QList<KNotePrintObject *> lst;
    const QString result = grantleePrint->notesToHtml(lst);
    QVERIFY(result.isEmpty());
    grantleePrint->deleteLater();
    grantleePrint = nullptr;
}

void KNotesGrantleePrintTest::shouldReturnEmptyStringWhenAddContentWithoutNotes()
{
    KNoteGrantleePrint *grantleePrint = new KNoteGrantleePrint;
    grantleePrint->setContent(QStringLiteral("foo"));
    QList<KNotePrintObject *> lst;
    const QString result = grantleePrint->notesToHtml(lst);
    QVERIFY(result.isEmpty());
    grantleePrint->deleteLater();
    grantleePrint = nullptr;
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

    KNoteGrantleePrint *grantleePrint = new KNoteGrantleePrint;
    grantleePrint->setContent(QStringLiteral("{% if notes %}{% for note in notes %}{{ note.%1 }}{% endfor %}{% endif %}").arg(variable));

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
    KNotePrintObject *obj = new KNotePrintObject(note);
    lst << obj;
    const QString html = grantleePrint->notesToHtml(lst);
    QCOMPARE(html, result);
    grantleePrint->deleteLater();
    grantleePrint = nullptr;
}

QTEST_MAIN(KNotesGrantleePrintTest)
