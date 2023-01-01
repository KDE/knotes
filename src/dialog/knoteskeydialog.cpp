/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knoteskeydialog.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KShortcutsEditor>
#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>

KNotesKeyDialog::KNotesKeyDialog(KActionCollection *globals, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Configure Shortcuts"));
    auto mainLayout = new QVBoxLayout(this);
    m_keyChooser = new KShortcutsEditor(globals, this);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNotesKeyDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNotesKeyDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, m_keyChooser, &KShortcutsEditor::allDefault);

    mainLayout->addWidget(m_keyChooser);
    mainLayout->addWidget(buttonBox);
    readConfig();
}

KNotesKeyDialog::~KNotesKeyDialog()
{
    writeConfig();
}

namespace
{
static const char myKNotesKeyDialogName[] = "KNotesKeyDialog";
}
void KNotesKeyDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(300, 200));
    KConfigGroup group(KSharedConfig::openStateConfig(), myKNotesKeyDialogName);
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void KNotesKeyDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myKNotesKeyDialogName);
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}

void KNotesKeyDialog::insert(KActionCollection *actions)
{
    m_keyChooser->addCollection(actions, i18n("Note Actions"));
}

void KNotesKeyDialog::save()
{
    m_keyChooser->save();
}
