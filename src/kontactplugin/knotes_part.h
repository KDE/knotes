/*
  This file is part of the KDE project

  SPDX-FileCopyrightText: 2002 Daniel Molkentin <molkentin@kde.org>
  SPDX-FileCopyrightText: 2004 Michael Brade <brade@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <AkonadiCore/Item>

#include <KParts/Part>
#include <KViewStateMaintainer>

#include <QListWidgetItem>
#include <QPointer>

class KNoteFindDialog;
class KNotesWidget;
namespace NoteShared
{
class NotesAkonadiTreeModel;
class NotesChangeRecorder;
}

namespace Akonadi
{
class ETMViewStateSaver;
}

class KCheckableProxyModel;
class KToggleAction;
namespace KDNSSD
{
class PublicService;
}

class KNotesPart : public KParts::Part
{
    Q_OBJECT

public:
    explicit KNotesPart(QObject *parent = nullptr);
    ~KNotesPart() override;

public Q_SLOTS:
    void newNote(const QString &name = QString(), const QString &text = QString());
    void newNoteFromClipboard(const QString &name = QString());
    QStringList notesList() const;

public:
    void updateConfig();
    void killNote(Akonadi::Item::Id id);
    void killNote(Akonadi::Item::Id id, bool force);

    QString name(Akonadi::Item::Id id) const;
    QString text(Akonadi::Item::Id id) const;

    void setName(Akonadi::Item::Id id, const QString &newName);
    void setText(Akonadi::Item::Id id, const QString &newText);

    QMap<QString, QString> notes() const;
    void popupRMB(QListWidgetItem *item, const QPoint &pos, const QPoint &globalPos);
    void editNote(Akonadi::Item::Id id);

    void updateClickMessage();
private Q_SLOTS:
    void editNote(QListWidgetItem *item);
    void editNote();

    void renameNote();

    void slotOnCurrentChanged();

    void killSelectedNotes();

    void slotPrintSelectedNotes();
    void slotPrintPreviewSelectedNotes();

    void slotNotePreferences();
    void slotPreferences();
    void slotMail();
    void slotSendToNetwork();
    void slotConfigUpdated();
    void slotSetAlarm();
    void slotNewNoteFromClipboard();
    void slotSaveAs();
    void slotUpdateReadOnly();

    void slotNoteCreationFinished(KJob *job);
    void slotRowInserted(const QModelIndex &parent, int start, int end);
    void slotItemChanged(const Akonadi::Item &id, const QSet<QByteArray> &set);
    void slotNoteSaved(KJob *);
    void slotDeleteNotesFinished(KJob *job);
    void slotItemRemoved(const Akonadi::Item &item);
    void slotOpenFindDialog();
    void slotSelectNote(Akonadi::Item::Id id);
    void slotCollectionChanged(const Akonadi::Collection &col, const QSet<QByteArray> &set);
    void slotItemFetchFinished(KJob *job);
    void slotNewNoteFromTextFile();

private:
    void fetchNotesFromCollection(const Akonadi::Collection &col);
    void updateNetworkListener();
    void printSelectedNotes(bool preview);
    KNotesWidget *mNotesWidget = nullptr;
    KDNSSD::PublicService *mPublisher = nullptr;
    QAction *mNoteEdit = nullptr;
    QAction *mNoteRename = nullptr;
    QAction *mNoteDelete = nullptr;
    QAction *mNotePrint = nullptr;
    QAction *mNotePrintPreview = nullptr;
    QAction *mNoteConfigure = nullptr;
    QAction *mNoteSendMail = nullptr;
    QAction *mNoteSendNetwork = nullptr;
    QAction *mNoteSetAlarm = nullptr;
    QAction *mNewNote = nullptr;
    QAction *mSaveAs = nullptr;
    QAction *mQuickSearchAction = nullptr;
    KToggleAction *mReadOnly = nullptr;
    NoteShared::NotesChangeRecorder *mNoteRecorder = nullptr;
    NoteShared::NotesAkonadiTreeModel *mNoteTreeModel = nullptr;
    QItemSelectionModel *mSelectionModel = nullptr;
    KCheckableProxyModel *mModelProxy = nullptr;
    KViewStateMaintainer<Akonadi::ETMViewStateSaver> *mModelState = nullptr;
    QPointer<KNoteFindDialog> mNoteFindDialog;
};

