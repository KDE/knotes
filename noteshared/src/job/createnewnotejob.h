/*
  Copyright (c) 2013-2016 Montel Laurent <montel@kde.org>

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

#ifndef CREATENEWNOTEJOB_H
#define CREATENEWNOTEJOB_H

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
    explicit CreateNewNoteJob(QObject *parent = Q_NULLPTR, QWidget *widget = Q_NULLPTR);
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
