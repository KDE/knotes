/*
  Copyright (c) 2013-2016 Montel Laurent <montel@kde.org>

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

#include "notesagentsettingsdialog.h"

#include "kdepim-version.h"
#include <QMenu>
#include <KHelpMenu>
#include <KLocalizedString>
#include <QIcon>
#include <KAboutData>
#include <KNotifyConfigWidget>

#include <QHBoxLayout>
#include <QTabWidget>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <KConfigGroup>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>

#include <config/notenetworkconfig.h>

NotesAgentSettingsDialog::NotesAgentSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18n("Configure Notes Agent"));
    setWindowIcon(QIcon::fromTheme(QStringLiteral("knotes")));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &NotesAgentSettingsDialog::reject);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &NotesAgentSettingsDialog::slotOkClicked);

    setModal(true);
    QWidget *mainWidget = new QWidget(this);
    QHBoxLayout *lay = new QHBoxLayout(mainWidget);

    QTabWidget *tab = new QTabWidget;
    lay->addWidget(tab);

    mNotify = new KNotifyConfigWidget(this);
    mNotify->setApplication(QStringLiteral("akonadi_notes_agent"));
    tab->addTab(mNotify, i18n("Notify"));

    mNetworkConfig = new NoteShared::NoteNetworkConfigWidget(this);
    tab->addTab(mNetworkConfig, i18n("Network"));
    mNetworkConfig->load();

    mainLayout->addWidget(mainWidget);
    mainLayout->addWidget(buttonBox);
    readConfig();

    KAboutData aboutData = KAboutData(
                               QStringLiteral("notesagent"),
                               i18n("Notes Agent"),
                               QStringLiteral(KDEPIM_VERSION),
                               i18n("Notes Agent."),
                               KAboutLicense::GPL_V2,
                               i18n("Copyright (C) 2013-2015 Laurent Montel"));

    aboutData.addAuthor(i18n("Laurent Montel"),
                        i18n("Maintainer"), QStringLiteral("montel@kde.org"));

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("knotes")));
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                            i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    KHelpMenu *helpMenu = new KHelpMenu(this, aboutData, true);
    //Initialize menu
    QMenu *menu = helpMenu->menu();
    helpMenu->action(KHelpMenu::menuAboutApp)->setIcon(QIcon::fromTheme(QStringLiteral("knotes")));
    buttonBox->button(QDialogButtonBox::Help)->setMenu(menu);
}

NotesAgentSettingsDialog::~NotesAgentSettingsDialog()
{
    writeConfig();
}

static const char myConfigGroupName[] = "NotesAgentSettingsDialog";
void NotesAgentSettingsDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), myConfigGroupName);

    const QSize size = group.readEntry("Size", QSize(500, 300));
    if (size.isValid()) {
        resize(size);
    }
}

void NotesAgentSettingsDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), myConfigGroupName);
    group.writeEntry("Size", size());
    group.sync();
}

void NotesAgentSettingsDialog::slotOkClicked()
{
    mNotify->save();
    mNetworkConfig->save();
    accept();
}
