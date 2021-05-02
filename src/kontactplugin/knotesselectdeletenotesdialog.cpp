/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knotesselectdeletenotesdialog.h"
#include "knotesiconview.h"

#include <KLocalizedString>
#include <KStandardGuiItem>

#include <KConfigGroup>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

KNotesSelectDeleteNotesListWidget::KNotesSelectDeleteNotesListWidget(QWidget *parent)
    : QListWidget(parent)
{
}

KNotesSelectDeleteNotesListWidget::~KNotesSelectDeleteNotesListWidget()
{
}

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

void KNotesSelectDeleteNotesDialog::readConfig()
{
    KConfigGroup grp(KSharedConfig::openStateConfig(), "KNotesSelectDeleteNotesDialog");
    const QSize size = grp.readEntry("Size", QSize(300, 200));
    if (size.isValid()) {
        resize(size);
    }
}

void KNotesSelectDeleteNotesDialog::writeConfig()
{
    KConfigGroup grp(KSharedConfig::openStateConfig(), "KNotesSelectDeleteNotesDialog");
    grp.writeEntry("Size", size());
    grp.sync();
}
