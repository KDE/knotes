/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2013 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#pragma once

#include <AkonadiCore/Item>

#include <KXMLGUIClient>

#include <QDomDocument>
#include <QPointer>
#include <QSessionManager>
#include <QWidget>

class KNote;
class KNoteFindDialog;
class KNotesAkonadiTray;
namespace NoteShared
{
class NotesChangeRecorder;
class NotesAkonadiTreeModel;
}

namespace KDNSSD
{
class PublicService;
}

class QMenu;

class KNotesApp : public QWidget, virtual public KXMLGUIClient
{
    Q_OBJECT
public:
    explicit KNotesApp(QWidget *parent = nullptr);
    ~KNotesApp() override;

    Q_REQUIRED_RESULT QString name(Akonadi::Item::Id id) const;
    Q_REQUIRED_RESULT QString text(Akonadi::Item::Id id) const;
public Q_SLOTS:
    void newNote(const QString &name = QString(), const QString &text = QString());
    void newNoteFromClipboard();
    void hideAllNotes() const;
    void showAllNotes() const;
    void showNote(Akonadi::Item::Id id) const;
    void hideNote(Akonadi::Item::Id id) const;
    void setName(Akonadi::Item::Id id, const QString &newName);
    void setText(Akonadi::Item::Id id, const QString &newText);

private:
    void showNote(KNote *note) const;

private Q_SLOTS:
    void slotCommitData(QSessionManager &);
    void slotPreferences();
    void slotConfigUpdated();
    void slotNoteDeleteFinished(KJob *);
    void slotRowInserted(const QModelIndex &, int, int end);
    void slotItemRemoved(const Akonadi::Item &item);
    void slotItemChanged(const Akonadi::Item &item, const QSet<QByteArray> &);
    void updateNoteActions();
    void slotActivateRequested(bool, const QPoint &pos);
    void slotSecondaryActivateRequested(const QPoint &);
    void slotPrintSelectedNotes();
    void slotQuit();
    void slotConfigureAccels();
    void slotShowNote();
    void slotWalkThroughNotes();
    void slotNoteKilled(Akonadi::Item::Id);
    void slotOpenFindDialog();

    void slotSelectNote(Akonadi::Item::Id);
    void slotCollectionChanged(const Akonadi::Collection &, const QSet<QByteArray> &);
    void slotItemFetchFinished(KJob *job);
    void slotDeleteSelectedNotes();

    void newNoteFromTextFile();

private:
    void saveNotes(bool force = false, bool sync = false);
    void updateNetworkListener();
    void updateSystray();
    void createNote(const Akonadi::Item &item);
    void fetchNotesFromCollection(const Akonadi::Collection &col);
    KXMLGUIFactory *m_guiFactory = nullptr;
    KXMLGUIBuilder *m_guiBuilder = nullptr;
    QDomDocument m_noteGUI;
    KNotesAkonadiTray *mTray = nullptr;
    QMenu *m_noteMenu = nullptr;
    QList<QAction *> m_noteActions;
    KDNSSD::PublicService *m_publisher = nullptr;
    QHash<Akonadi::Item::Id, KNote *> mNotes;
    NoteShared::NotesChangeRecorder *mNoteRecorder = nullptr;
    NoteShared::NotesAkonadiTreeModel *mNoteTreeModel = nullptr;
    QPointer<KNoteFindDialog> mFindDialog;
    bool mDebugAkonadiSearch = false;
};

