/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QWidget>

#include "noteshared_export.h"

class KJob;

namespace NoteShared
{
class CreateNewNoteJobPrivate;
class NOTESHARED_EXPORT CreateNewNoteJob : public QObject
{
    Q_OBJECT
public:
    explicit CreateNewNoteJob(QObject *parent = nullptr, QWidget *widget = nullptr);
    ~CreateNewNoteJob() override;

    void setNote(const QString &name, const QString &text);

    void start();

    void setRichText(bool richText);

Q_SIGNALS:
    void selectNewCollection();

private:
    void slotNoteCreationFinished(KJob *);

    void slotFetchCollection(KJob *job);
    void slotCollectionModifyFinished(KJob *job);
    void slotSelectNewCollection();
    void createFetchCollectionJob(bool useSettings);
    QString mTitle;
    QString mText;
    bool mRichText = false;
    QWidget *const mWidget;
};
}
