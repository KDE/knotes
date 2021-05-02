/*
  This file is part of Kontact
  SPDX-FileCopyrightText: 2002 Daniel Molkentin <molkentin@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "knotes_plugin.h"
#include "apps/knotes_options.h"
#include "knotes_part.h"
#include "knotesglobalconfig.h"
#include "summarywidget.h"
#include "utils/knoteutils.h"
#include <KCalUtils/ICalDrag>
#include <KCalUtils/VCalDrag>
#include <KCalendarCore/FileStorage>

#include "knotes-version.h"

using namespace KCalUtils;
using namespace KCalendarCore;

#include <KContacts/VCardDrag>

#include <KontactInterface/Core>

#include "knotes_kontact_plugin_debug.h"
#include <KActionCollection>
#include <KLocalizedString>
#include <QAction>
#include <QIcon>
#include <QTimeZone>

#include <QDropEvent>
#include <QStandardPaths>

EXPORT_KONTACT_PLUGIN_WITH_JSON(KNotesPlugin, "knotesplugin.json")

KNotesPlugin::KNotesPlugin(KontactInterface::Core *core, const QVariantList &)
    : KontactInterface::Plugin(core, core, "knotes")
{
    setComponentName(QStringLiteral("knotes"), i18n("KNotes"));

    auto action = new QAction(QIcon::fromTheme(QStringLiteral("knotes")), i18nc("@action:inmenu", "New Popup Note..."), this);
    actionCollection()->addAction(QStringLiteral("new_note"), action);
    connect(action, &QAction::triggered, this, &KNotesPlugin::slotNewNote);
    actionCollection()->setDefaultShortcut(action, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N));
    // action->setHelpText(
    //            i18nc( "@info:status", "Create new popup note" ) );
    action->setWhatsThis(i18nc("@info:whatsthis", "You will be presented with a dialog where you can create a new popup note."));
    insertNewAction(action);

    mUniqueAppWatcher = new KontactInterface::UniqueAppWatcher(new KontactInterface::UniqueAppHandlerFactory<KNotesUniqueAppHandler>(), this);
}

KNotesPlugin::~KNotesPlugin()
{
}

bool KNotesPlugin::isRunningStandalone() const
{
    return mUniqueAppWatcher->isRunningStandalone();
}

KParts::Part *KNotesPlugin::createPart()
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
                                      xi18nc("@info:credit", "Copyright © 2003–2021 Kontact authors"));

    aboutData.addAuthor(xi18nc("@info:credit", "Laurent Montel"), xi18nc("@info:credit", "Current Maintainer"), QStringLiteral("montel@kde.org"));

    aboutData.addAuthor(xi18nc("@info:credit", "Michael Brade"), xi18nc("@info:credit", "Previous Maintainer"), QStringLiteral("brade@kde.org"));
    aboutData.addAuthor(xi18nc("@info:credit", "Tobias Koenig"), xi18nc("@info:credit", "Developer"), QStringLiteral("tokoe@kde.org"));

    return aboutData;
}

bool KNotesPlugin::canDecodeMimeData(const QMimeData *mimeData) const
{
    return mimeData->hasText() || KContacts::VCardDrag::canDecode(mimeData) || ICalDrag::canDecode(mimeData);
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
        static_cast<KNotesPart *>(part())->newNote(i18nc("@item", "Meeting"), attendees.join(QLatin1String(", ")));
        return;
    }

    if (KCalUtils::ICalDrag::canDecode(md)) {
        KCalendarCore::MemoryCalendar::Ptr cal(new KCalendarCore::MemoryCalendar(QTimeZone::systemTimeZone()));
        if (KCalUtils::ICalDrag::fromMimeData(md, cal)) {
            KCalendarCore::Incidence::List incidences = cal->incidences();
            Q_ASSERT(incidences.count());
            if (!incidences.isEmpty()) {
                event->accept();
                KCalendarCore::Incidence::Ptr i = incidences.first();
                QString summary;
                if (i->type() == KCalendarCore::Incidence::TypeJournal) {
                    summary = i18nc("@item", "Note: %1", i->summary());
                } else {
                    summary = i->summary();
                }
                static_cast<KNotesPart *>(part())->newNote(i18nc("@item", "Note: %1", summary), i->description());
                return;
            }
        }
    }
    if (md->hasText()) {
        static_cast<KNotesPart *>(part())->newNote(i18nc("@item", "New Note"), md->text());
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
