/*
  This file is part of Kontact.

  SPDX-FileCopyrightText: 2003 Tobias Koenig <tokoe@kde.org>
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#include "summarywidget.h"
#include "akonadi/notesakonaditreemodel.h"
#include "akonadi/noteschangerecorder.h"
#include "attributes/notedisplayattribute.h"
#include "knotesinterface.h"

#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/Session>
#include <AkonadiWidgets/ETMViewStateSaver>

#include <KontactInterface/Core>
#include <KontactInterface/Plugin>

#include <KCheckableProxyModel>
#include <KIconEffect>
#include <KIconLoader>
#include <KLocalizedString>
#include <KMime/Message>
#include <KSharedConfig>
#include <KUrlLabel>

#include <QItemSelectionModel>
#include <QLabel>
#include <QMenu>
#include <QVBoxLayout>

KNotesSummaryWidget::KNotesSummaryWidget(KontactInterface::Plugin *plugin, QWidget *parent)
    : KontactInterface::Summary(parent)
    , mPlugin(plugin)
    , mInProgress(false)
{
    mDefaultPixmap = KIconLoader::global()->loadIcon(QStringLiteral("knotes"), KIconLoader::Desktop);
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(3, 3, 3, 3);

    QWidget *header = createHeader(this, QStringLiteral("view-pim-notes"), i18n("Popup Notes"));
    mainLayout->addWidget(header);

    mLayout = new QGridLayout();
    mainLayout->addLayout(mLayout);
    mLayout->setSpacing(3);
    mLayout->setRowStretch(6, 1);

    KIconLoader loader(QStringLiteral("knotes"));

    mPixmap = loader.loadIcon(QStringLiteral("knotes"), KIconLoader::Small);

    auto session = new Akonadi::Session("KNotes Session", this);
    mNoteRecorder = new NoteShared::NotesChangeRecorder(this);
    mNoteRecorder->changeRecorder()->setSession(session);
    mNoteTreeModel = new NoteShared::NotesAkonadiTreeModel(mNoteRecorder->changeRecorder(), this);

    connect(mNoteTreeModel, &NoteShared::NotesAkonadiTreeModel::rowsInserted, this, &KNotesSummaryWidget::updateFolderList);

    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemChanged, this, &KNotesSummaryWidget::updateFolderList);

    connect(mNoteRecorder->changeRecorder(), &Akonadi::Monitor::itemRemoved, this, &KNotesSummaryWidget::updateFolderList);

    mSelectionModel = new QItemSelectionModel(mNoteTreeModel);
    mModelProxy = new KCheckableProxyModel(this);
    mModelProxy->setSelectionModel(mSelectionModel);
    mModelProxy->setSourceModel(mNoteTreeModel);

    KSharedConfigPtr _config = KSharedConfig::openConfig(QStringLiteral("kcmknotessummaryrc"));

    mModelState = new KViewStateMaintainer<Akonadi::ETMViewStateSaver>(_config->group("CheckState"), this);
    mModelState->setSelectionModel(mSelectionModel);
}

KNotesSummaryWidget::~KNotesSummaryWidget()
{
}

void KNotesSummaryWidget::updateFolderList()
{
    if (mInProgress) {
        return;
    }
    mInProgress = true;
    qDeleteAll(mLabels);
    mLabels.clear();
    int counter = 0;

    mModelState->restoreState();
    displayNotes(QModelIndex(), counter);
    mInProgress = false;

    if (counter == 0) {
        auto label = new QLabel(i18n("No notes found"), this);
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        mLayout->addWidget(label, 0, 0);
        mLabels.append(label);
    }
    QList<QLabel *>::const_iterator lit;
    QList<QLabel *>::const_iterator lend(mLabels.constEnd());
    for (lit = mLabels.constBegin(); lit != lend; ++lit) {
        (*lit)->show();
    }
}

void KNotesSummaryWidget::displayNotes(const QModelIndex &parent, int &counter)
{
    const int nbCol = mModelProxy->rowCount(parent);
    for (int i = 0; i < nbCol; ++i) {
        const QModelIndex child = mModelProxy->index(i, 0, parent);
        const auto item = mModelProxy->data(child, Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();
        if (item.isValid()) {
            createNote(item, counter);
            ++counter;
        }
        displayNotes(child, counter);
    }
}

void KNotesSummaryWidget::slotPopupMenu(const QString &note)
{
    QMenu popup(this);
    const QAction *modifyNoteAction =
        popup.addAction(KIconLoader::global()->loadIcon(QStringLiteral("document-edit"), KIconLoader::Small), i18n("Modify Note..."));
    popup.addSeparator();
    const QAction *deleteNoteAction =
        popup.addAction(KIconLoader::global()->loadIcon(QStringLiteral("edit-delete"), KIconLoader::Small), i18n("Delete Note..."));

    const QAction *ret = popup.exec(QCursor::pos());
    if (ret == deleteNoteAction) {
        deleteNote(note);
    } else if (ret == modifyNoteAction) {
        slotSelectNote(note);
    }
}

void KNotesSummaryWidget::deleteNote(const QString &note)
{
    org::kde::kontact::KNotes knotes(QStringLiteral("org.kde.kontact"), QStringLiteral("/KNotes"), QDBusConnection::sessionBus());
    knotes.killNote(note.toLongLong());
}

void KNotesSummaryWidget::createNote(const Akonadi::Item &item, int counter)
{
    if (!item.hasPayload<KMime::Message::Ptr>()) {
        return;
    }

    auto noteMessage = item.payload<KMime::Message::Ptr>();
    if (!noteMessage) {
        return;
    }
    const KMime::Headers::Subject *const subject = noteMessage->subject(false);
    const QString subStr = subject ? subject->asUnicodeString() : QString();
    auto urlLabel = new KUrlLabel(QString::number(item.id()), subStr, this);

    urlLabel->installEventFilter(this);
    urlLabel->setAlignment(Qt::AlignLeft);
    urlLabel->setWordWrap(true);
    connect(urlLabel, &KUrlLabel::leftClickedUrl, this, [this, urlLabel]() {
        slotSelectNote(urlLabel->url());
    });

    connect(urlLabel, &KUrlLabel::rightClickedUrl, this, [this, urlLabel]() {
        slotPopupMenu(urlLabel->url());
    });
    mLayout->addWidget(urlLabel, counter, 1);

    QColor color;
    if (item.hasAttribute<NoteShared::NoteDisplayAttribute>()) {
        color = item.attribute<NoteShared::NoteDisplayAttribute>()->backgroundColor();
    }
    // Folder icon.
    KIconEffect effect;
    QPixmap pixmap = effect.apply(mDefaultPixmap, KIconEffect::Colorize, 1, color, false);

    auto label = new QLabel(this);
    label->setAlignment(Qt::AlignVCenter);
    QIcon icon(pixmap);
    label->setPixmap(icon.pixmap(label->height() / 1.5));
    label->setMaximumWidth(label->minimumSizeHint().width());
    mLayout->addWidget(label, counter, 0);
    mLabels.append(label);
    mLabels.append(urlLabel);
}

void KNotesSummaryWidget::updateSummary(bool force)
{
    Q_UNUSED(force)
    updateFolderList();
}

void KNotesSummaryWidget::slotSelectNote(const QString &note)
{
    if (!mPlugin->isRunningStandalone()) {
        mPlugin->core()->selectPlugin(mPlugin);
    } else {
        mPlugin->bringToForeground();
    }

    org::kde::kontact::KNotes knotes(QStringLiteral("org.kde.kontact"), QStringLiteral("/KNotes"), QDBusConnection::sessionBus());
    knotes.editNote(note.toLongLong());
}

bool KNotesSummaryWidget::eventFilter(QObject *obj, QEvent *e)
{
    if (obj->inherits("KUrlLabel")) {
        auto label = static_cast<KUrlLabel *>(obj);
        if (e->type() == QEvent::Enter) {
            Q_EMIT message(i18n("Read Popup Note: \"%1\"", label->text()));
        } else if (e->type() == QEvent::Leave) {
            Q_EMIT message(QString());
        }
    }

    return KontactInterface::Summary::eventFilter(obj, e);
}

QStringList KNotesSummaryWidget::configModules() const
{
    return QStringList() << QStringLiteral("kcmknotessummary.desktop");
}
