/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "notesagentsettingsdialog.h"

#include "knotes-version.h"
#include <KAboutData>
#include <KHelpMenu>
#include <KLocalizedString>
#include <KNotifyConfigWidget>
#include <QIcon>
#include <QMenu>

#include <KConfigGroup>
#include <KSharedConfig>
#include <KWindowConfig>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWindow>

#include <config/notenetworkconfig.h>

NotesAgentSettingsDialog::NotesAgentSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , mNotify(new KNotifyConfigWidget(this))
    , mNetworkConfig(new NoteShared::NoteNetworkConfigWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Configure Notes Agent"));
    setWindowIcon(QIcon::fromTheme(QStringLiteral("knotes")));
    auto mainLayout = new QVBoxLayout(this);
    setModal(true);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &NotesAgentSettingsDialog::reject);
    connect(okButton, &QPushButton::clicked, this, &NotesAgentSettingsDialog::slotOkClicked);

    auto tab = new QTabWidget(this);

    mNotify->setApplication(QStringLiteral("akonadi_notes_agent"));
    tab->addTab(mNotify, i18n("Notify"));

    tab->addTab(mNetworkConfig, i18n("Network"));
    mNetworkConfig->load();

    mainLayout->addWidget(tab);
    mainLayout->addWidget(buttonBox);
    readConfig();

    KAboutData aboutData = KAboutData(QStringLiteral("notesagent"),
                                      i18n("Notes Agent"),
                                      QStringLiteral(KNOTES_VERSION),
                                      i18n("Notes Agent."),
                                      KAboutLicense::GPL_V2,
                                      i18n("Copyright (C) 2013-%1 Laurent Montel", QStringLiteral("2022")));

    aboutData.addAuthor(i18n("Laurent Montel"), i18n("Maintainer"), QStringLiteral("montel@kde.org"));

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("knotes")));
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    auto helpMenu = new KHelpMenu(this, aboutData, true);
    // Initialize menu
    QMenu *menu = helpMenu->menu();
    helpMenu->action(KHelpMenu::menuAboutApp)->setIcon(QIcon::fromTheme(QStringLiteral("knotes")));
    buttonBox->button(QDialogButtonBox::Help)->setMenu(menu);
}

NotesAgentSettingsDialog::~NotesAgentSettingsDialog()
{
    writeConfig();
}

static const char myConfigGroupName[] = "NotesAgentSettingsDialog";
void NotesAgentSettingsDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(500, 300));
    KConfigGroup group(KSharedConfig::openStateConfig(), myConfigGroupName);
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void NotesAgentSettingsDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myConfigGroupName);
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}

void NotesAgentSettingsDialog::slotOkClicked()
{
    mNotify->save();
    mNetworkConfig->save();
    accept();
}
