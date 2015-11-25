/*
  Copyright (c) 2015 Montel Laurent <montel@kde.org>

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

#include "knotegrantleeprint.h"
#include "knoteprintobject.h"
#include <KLocalizedString>
#include <grantlee/context.h>
#include <grantlee/engine.h>

KNoteGrantleePrint::KNoteGrantleePrint(QObject *parent)
    : QObject(parent)
{
    mEngine = new Grantlee::Engine;
}

KNoteGrantleePrint::KNoteGrantleePrint(const QString &themePath, QObject *parent)
    : QObject(parent)
{
    mEngine = new Grantlee::Engine;
    mTemplateLoader =  QSharedPointer<Grantlee::FileSystemTemplateLoader>(new Grantlee::FileSystemTemplateLoader);

    mTemplateLoader->setTemplateDirs(QStringList() << themePath);
    mEngine->addTemplateLoader(mTemplateLoader);

    mSelfcontainedTemplate = mEngine->loadByName(QStringLiteral("theme.html"));
    if (mSelfcontainedTemplate->error()) {
        mErrorMessage = mSelfcontainedTemplate->errorString() + QLatin1String("<br>");
    }
}

KNoteGrantleePrint::~KNoteGrantleePrint()
{
    mEngine->deleteLater();
    mEngine = 0;
}

QString KNoteGrantleePrint::errorMessage() const
{
    return mErrorMessage;
}

void KNoteGrantleePrint::setContent(const QString &content)
{
    mSelfcontainedTemplate = mEngine->newTemplate(content, QStringLiteral("content"));
    if (mSelfcontainedTemplate->error()) {
        mErrorMessage = mSelfcontainedTemplate->errorString() + QLatin1String("<br>");
    }
}

QString KNoteGrantleePrint::notesToHtml(const QList<KNotePrintObject *> &lst)
{
    if (lst.isEmpty()) {
        return QString();
    }
    QVariantList notes;
    notes.reserve(lst.count());
    Q_FOREACH (KNotePrintObject *n, lst) {
        notes << QVariant::fromValue(static_cast<QObject *>(n));
    }
    Grantlee::Context c;
    c.insert(QStringLiteral("notes"), notes);
    c.insert(QStringLiteral("alarm_i18n"), i18n("Alarm:"));
    c.insert(QStringLiteral("note_is_locked_i18n"), i18n("Note is locked"));

    const QString htmlText = mSelfcontainedTemplate->render(&c);
    return htmlText;
}
