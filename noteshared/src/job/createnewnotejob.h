/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef CREATENEWNOTEJOB_H
#define CREATENEWNOTEJOB_H

#include <QWidget>

#include "noteshared_export.h"
class KJob;

namespace NoteShared {
class CreateNewNoteJobPrivate;
class NOTESHARED_EXPORT CreateNewNoteJob : public QObject
{
    Q_OBJECT
public:
    explicit CreateNewNoteJob(QObject *parent = nullptr, QWidget *widget = nullptr);
    ~CreateNewNoteJob();

    void setNote(const QString &name, const QString &text);

    void start();

    void setRichText(bool richText);

Q_SIGNALS:
    void selectNewCollection();

private Q_SLOTS:
    void slotNoteCreationFinished(KJob *);

    void slotFetchCollection(KJob *job);
    void slotCollectionModifyFinished(KJob *job);
    void slotSelectNewCollection();
private:
    void createFetchCollectionJob(bool useSettings);
    CreateNewNoteJobPrivate *const d;
};
}

#endif // CREATENEWNOTEJOB_H
