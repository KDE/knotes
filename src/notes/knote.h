/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2013 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#pragma once

#include "knoteinterface.h"
#include <config-knotes.h>

#include <AkonadiCore/Item>

#include <KSharedConfig>
#include <KXMLGUIClient>

#include <QDomDocument>
#include <QFrame>

class KNoteButton;
class KNoteDisplaySettings;
class KNoteEdit;

class KSelectAction;
class KToggleAction;
class KToolBar;

class QLabel;
class QMenu;
class QSizeGrip;
class QVBoxLayout;
class QEvent;
class KNote : public QFrame, virtual public KXMLGUIClient, public KNoteInterface
{
    Q_OBJECT
public:
    explicit KNote(const QDomDocument &buildDoc, const Akonadi::Item &item, bool allowAkonadiSearchDebug = false, QWidget *parent = nullptr);
    ~KNote() override;

    void setChangeItem(const Akonadi::Item &item, const QSet<QByteArray> &set = QSet<QByteArray>());
    void saveNote(bool force = false, bool sync = false);

    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT QString text() const;
    Q_REQUIRED_RESULT Akonadi::Item::Id noteId() const;

    Q_REQUIRED_RESULT Akonadi::Item item() const;

    void setName(const QString &name);
    void setText(const QString &text);

    Q_REQUIRED_RESULT bool isModified() const;
    Q_REQUIRED_RESULT bool isDesktopAssigned() const;

    void toDesktop(int desktop);

public Q_SLOTS:
    void slotRename();
    void slotKill(bool force = false);
    void slotClose();

Q_SIGNALS:
    void sigRequestNewNote();
    void sigShowNextNote();
    void sigNameChanged(const QString &);
    void sigColorChanged();
    void sigKillNote(Akonadi::Item::Id);

protected:
    void contextMenuEvent(QContextMenuEvent *) override;
    void showEvent(QShowEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void closeEvent(QCloseEvent *) override;
    void dropEvent(QDropEvent *) override;
    void dragEnterEvent(QDragEnterEvent *) override;

    bool event(QEvent *) override;
    bool eventFilter(QObject *, QEvent *) override;

private Q_SLOTS:
    void slotUpdateReadOnly();

    void slotSend();
    void slotMail();
    void slotPrint();
    void slotPrintPreview();
    void slotSaveAs();

    void slotSetAlarm();

    void slotPreferences();
    void slotPopupActionToDesktop(QAction *act);

    void slotApplyConfig();

    void slotUpdateShowInTaskbar();
    void slotUpdateDesktopActions();

    void slotKeepAbove();
    void slotKeepBelow();

    void slotRequestNewNote();
    void slotNoteSaved(KJob *job);
    void slotDebugAkonadiSearch();

private:
    void updateKeepAboveBelow(bool save = true);
    void buildGui();
    void createActions();
    void createNoteEditor(const QString &configFile);
    void createNoteFooter();
    void createNoteHeader();
    void prepare();

    void updateFocus();
    void updateLayout();
    void updateLabelAlignment();

    void setColor(const QColor &, const QColor &);
    void print(bool preview);
    void setDisplayDefaultValue();

private:
    void loadNoteContent(const Akonadi::Item &item);
    void updateAllAttributes();
    void saveNoteContent();
    Akonadi::Item mItem;
    QVBoxLayout *m_noteLayout = nullptr;
    QLabel *m_label = nullptr;
    QSizeGrip *m_grip = nullptr;
    KNoteButton *m_button = nullptr;
    KToolBar *m_tool = nullptr;
    KNoteEdit *m_editor = nullptr;

    QMenu *m_menu = nullptr;

    KToggleAction *m_readOnly = nullptr;

#if KDEPIM_HAVE_X11
    KSelectAction *m_toDesktop = nullptr;
#endif
    KToggleAction *m_keepAbove = nullptr;
    KToggleAction *m_keepBelow = nullptr;

    KSharedConfig::Ptr m_kwinConf;

    KNoteDisplaySettings *mDisplayAttribute = nullptr;
    bool mAllowDebugAkonadiSearch;
    QPoint mOrigPos;
};

