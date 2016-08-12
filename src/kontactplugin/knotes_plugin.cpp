/*
  This file is part of Kontact
  Copyright (c) 2002 Daniel Molkentin <molkentin@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "knotes_plugin.h"
#include "knotes_part.h"
#include "apps/knotes_options.h"
#include "utils/knoteutils.h"
#include "summarywidget.h"
#include "knotesglobalconfig.h"
#include <KCalUtils/ICalDrag>
#include <KCalUtils/VCalDrag>
#include <KCalCore/FileStorage>

#include "knotes-version.h"

#include <Libkdepim/MaillistDrag>
using namespace KPIM;
using namespace KCalUtils;
using namespace KCalCore;

#include <KContacts/VCardDrag>

#include <KontactInterface/Core>

#include <QAction>
#include <KActionCollection>
#include "knotes_kontact_plugin_debug.h"
#include <QIcon>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSystemTimeZones>
#include <QTemporaryFile>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDropEvent>
#include <QStandardPaths>

EXPORT_KONTACT_PLUGIN(KNotesPlugin, knotes)

KNotesPlugin::KNotesPlugin(KontactInterface::Core *core, const QVariantList &)
    : KontactInterface::Plugin(core, core, "knotes")
{
    setComponentName(QStringLiteral("knotes"), QStringLiteral("knotes"));

    QAction *action =
        new QAction(QIcon::fromTheme(QStringLiteral("knotes")),
                    i18nc("@action:inmenu", "New Popup Note..."), this);
    actionCollection()->addAction(QStringLiteral("new_note"), action);
    connect(action, &QAction::triggered, this, &KNotesPlugin::slotNewNote);
    actionCollection()->setDefaultShortcut(action, QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N));
    //action->setHelpText(
    //            i18nc( "@info:status", "Create new popup note" ) );
    action->setWhatsThis(
        i18nc("@info:whatsthis",
              "You will be presented with a dialog where you can create a new popup note."));
    insertNewAction(action);

    mUniqueAppWatcher = new KontactInterface::UniqueAppWatcher(
        new KontactInterface::UniqueAppHandlerFactory<KNotesUniqueAppHandler>(), this);

}

KNotesPlugin::~KNotesPlugin()
{
}

bool KNotesPlugin::isRunningStandalone() const
{
    return mUniqueAppWatcher->isRunningStandalone();
}

KParts::ReadOnlyPart *KNotesPlugin::createPart()
{
    return new KNotesPart(this);
}

KontactInterface::Summary *KNotesPlugin::createSummaryWidget(QWidget *parentWidget)
{
    return new KNotesSummaryWidget(this, parentWidget);
}

const KAboutData KNotesPlugin::aboutData()
{
    KAboutData aboutData = KAboutData(QStringLiteral("knotes"),
                                      xi18nc("@title", "KNotes"),
                                      QStringLiteral(KNOTES_VERSION),
                                      xi18nc("@title", "Popup Notes"),
                                      KAboutLicense::GPL_V2,
                                      xi18nc("@info:credit", "Copyright © 2003–2016 Kontact authors"));

    aboutData.addAuthor(xi18nc("@info:credit", "Laurent Montel"),
                        xi18nc("@info:credit", "Current Maintainer"),
                        QStringLiteral("montel@kde.org"));

    aboutData.addAuthor(xi18nc("@info:credit", "Michael Brade"),
                        xi18nc("@info:credit", "Previous Maintainer"),
                        QStringLiteral("brade@kde.org"));
    aboutData.addAuthor(xi18nc("@info:credit", "Tobias Koenig"),
                        xi18nc("@info:credit", "Developer"),
                        QStringLiteral("tokoe@kde.org"));

    return aboutData;
}

bool KNotesPlugin::canDecodeMimeData(const QMimeData *mimeData) const
{
    return
        mimeData->hasText() ||
        MailList::canDecode(mimeData) ||
        KContacts::VCardDrag::canDecode(mimeData) ||
        ICalDrag::canDecode(mimeData);
}

void KNotesPlugin::processDropEvent(QDropEvent *event)
{
    const QMimeData *md = event->mimeData();

    if (KContacts::VCardDrag::canDecode(md)) {
        KContacts::Addressee::List contacts;

        KContacts::VCardDrag::fromMimeData(md, contacts);

        KContacts::Addressee::List::ConstIterator it;

        QStringList attendees;
        KContacts::Addressee::List::ConstIterator end(contacts.constEnd());
        for (it = contacts.constBegin(); it != end; ++it) {
            const QString email = (*it).fullEmail();
            if (email.isEmpty()) {
                attendees.append((*it).realName() + QLatin1String("<>"));
            } else {
                attendees.append(email);
            }
        }
        event->accept();
        static_cast<KNotesPart *>(part())->newNote(
            i18nc("@item", "Meeting"), attendees.join(QStringLiteral(", ")));
        return;
    }

    if (KCalUtils::ICalDrag::canDecode(md)) {
        KCalCore::MemoryCalendar::Ptr cal(new KCalCore::MemoryCalendar(KSystemTimeZones::local()));
        if (KCalUtils::ICalDrag::fromMimeData(md, cal)) {
            KCalCore::Incidence::List incidences = cal->incidences();
            Q_ASSERT(incidences.count());
            if (!incidences.isEmpty()) {
                event->accept();
                KCalCore::Incidence::Ptr i = incidences.first();
                QString summary;
                if (i->type() == KCalCore::Incidence::TypeJournal) {
                    summary = i18nc("@item", "Note: %1", i->summary());
                } else {
                    summary = i->summary();
                }
                static_cast<KNotesPart *>(part())->
                newNote(i18nc("@item", "Note: %1", summary), i->description());
                return;
            }
        }
    }
    if (md->hasText()) {
        static_cast<KNotesPart *>(part())->newNote(
            i18nc("@item", "New Note"), md->text());
        return;
    }

    if (MailList::canDecode(md)) {
        MailList mails = MailList::fromMimeData(md);
        event->accept();
        if (mails.count() != 1) {
            KMessageBox::sorry(
                core(),
                i18nc("@info", "Dropping multiple mails is not supported."));
        } else {
            MailSummary mail = mails.first();
            const QString txt = i18nc("@item", "From: %1\nTo: %2\nSubject: %3",
                                      mail.from(), mail.to(), mail.subject());
            static_cast<KNotesPart *>(part())->newNote(
                i18nc("@item", "Mail: %1", mail.subject()), txt);
        }
        return;
    }

    qCWarning(KNOTES_KONTACT_PLUGIN_LOG) << QStringLiteral("Cannot handle drop events of type '%1'.").arg(event->mimeData()->formats().join(QLatin1Char(';')));
}

void KNotesPlugin::shortcutChanged()
{
    if (part()) {
        static_cast<KNotesPart *>(part())->updateClickMessage();
    }
}

// private slots

void KNotesPlugin::slotNewNote()
{
    if (part()) {
        static_cast<KNotesPart *>(part())->newNote();
        core()->selectPlugin(this);
    }
}

void KNotesUniqueAppHandler::loadCommandLineOptions(QCommandLineParser *parser)
{
    knotesOptions(parser);
}

int KNotesUniqueAppHandler::activate(const QStringList &args, const QString &workingDir)
{
    qCDebug(KNOTES_KONTACT_PLUGIN_LOG);
    // Ensure part is loaded
    (void)plugin()->part();

    return KontactInterface::UniqueAppHandler::activate(args, workingDir);
}

#include "knotes_plugin.moc"

