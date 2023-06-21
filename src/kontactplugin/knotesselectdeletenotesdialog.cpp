/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knotesselectdeletenotesdialog.h"
#include "knotesiconview.h"

#include <KLocalizedString>
#include <KStandardGuiItem>

#include <KConfigGroup>
#include <KSharedConfig>
#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>

KNotesSelectDeleteNotesListWidget::KNotesSelectDeleteNotesListWidget(QWidget *parent)
    : QListWidget(parent)
{
}

KNotesSelectDeleteNotesListWidget::~KNotesSelectDeleteNotesListWidget() = default;

void KNotesSelectDeleteNotesListWidget::setItems(const QList<KNotesIconViewItem *> &items)
{
    for (KNotesIconViewItem *item : items) {
        auto i = new QListWidgetItem(this);
        if (item->readOnly()) {
            i->setText(item->realName() + QLatin1Char(' ') + i18n("(note locked, it will not removed)"));
            i->setForeground(Qt::red);
        } else {
            i->setText(item->realName());
        }
    }
}

KNotesSelectDeleteNotesDialog::KNotesSelectDeleteNotesDialog(const QList<KNotesIconViewItem *> &items, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Confirm Delete"));
    auto mainLayout = new QVBoxLayout(this);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNotesSelectDeleteNotesDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNotesSelectDeleteNotesDialog::reject);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(true);
    setModal(true);
    auto w = new QWidget;
    auto lay = new QVBoxLayout(w);
    lay->setContentsMargins(0, 0, 0, 0);
    auto lab = new QLabel(i18np("Do you really want to delete this note?", "Do you really want to delete these %1 notes?", items.count()), this);
    lay->addWidget(lab);
    mSelectedListWidget = new KNotesSelectDeleteNotesListWidget(this);
    lay->addWidget(mSelectedListWidget);
    mainLayout->addWidget(w);
    mSelectedListWidget->setItems(items);
    okButton->setText(KStandardGuiItem::del().text());

    mainLayout->addWidget(buttonBox);
    readConfig();
}

KNotesSelectDeleteNotesDialog::~KNotesSelectDeleteNotesDialog()
{
    writeConfig();
}

namespace
{
static const char myKNotesSelectDeleteNotesDialogName[] = "KNotesSelectDeleteNotesDialog";
}
void KNotesSelectDeleteNotesDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(300, 200));
    KConfigGroup group(KSharedConfig::openStateConfig(), myKNotesSelectDeleteNotesDialogName);
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void KNotesSelectDeleteNotesDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myKNotesSelectDeleteNotesDialogName);
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}

#include "moc_knotesselectdeletenotesdialog.cpp"
