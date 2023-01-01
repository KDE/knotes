/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knotesimpleconfigdialog.h"
#include "knoteconfigdialog.h"
#include "knotedisplayconfigwidget.h"
#include "knoteeditorconfigwidget.h"
#include <config-knotes.h>

#include "attributes/notedisplayattribute.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWindow>

KNoteSimpleConfigDialog::KNoteSimpleConfigDialog(const QString &title, QWidget *parent)
    : QDialog(parent)
{
    auto mainLayout = new QVBoxLayout(this);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNoteSimpleConfigDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNoteSimpleConfigDialog::reject);

    mTabWidget = new QTabWidget;

    mEditorConfigWidget = new KNoteEditorConfigWidget(this);
    mTabWidget->addTab(mEditorConfigWidget, i18n("Editor Settings"));

    mDisplayConfigWidget = new KNoteDisplayConfigWidget(true, this);
    mTabWidget->addTab(mDisplayConfigWidget, i18n("Display Settings"));

    mainLayout->addWidget(mTabWidget);
    mainLayout->addWidget(buttonBox);

    readConfig();
}

KNoteSimpleConfigDialog::~KNoteSimpleConfigDialog()
{
    writeConfig();
}

void KNoteSimpleConfigDialog::load(Akonadi::Item &item, bool isRichText)
{
    auto attr = item.attribute<NoteShared::NoteDisplayAttribute>();
    mEditorConfigWidget->load(attr, isRichText);
    mDisplayConfigWidget->load(attr);
}

void KNoteSimpleConfigDialog::slotUpdateCaption(const QString &name)
{
    setWindowTitle(name);
}

void KNoteSimpleConfigDialog::save(Akonadi::Item &item, bool &isRichText)
{
    auto attr = item.attribute<NoteShared::NoteDisplayAttribute>(Akonadi::Item::AddIfMissing);
    mEditorConfigWidget->save(attr, isRichText);
    mDisplayConfigWidget->save(attr);
}

namespace
{
static const char myKNoteSimpleConfigDialogName[] = "KNoteSimpleConfigDialog";
}
void KNoteSimpleConfigDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(600, 400));
    KConfigGroup group(KSharedConfig::openStateConfig(), myKNoteSimpleConfigDialogName);
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void KNoteSimpleConfigDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myKNoteSimpleConfigDialogName);
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}
