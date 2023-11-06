/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knoteprintselectednotesdialog.h"
#include "knoteprintobject.h"
#include "notes/knote.h"
#include "print/knoteprintselectthemecombobox.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <QIcon>

#include <KSharedConfig>
#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>

KNotePrintSelectedNotesDialog::KNotePrintSelectedNotesDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Select notes"));
    auto mainLayout = new QVBoxLayout(this);
    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *okButton = mButtonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    mUser1Button = new QPushButton;
    mButtonBox->addButton(mUser1Button, QDialogButtonBox::ActionRole);
    connect(mButtonBox, &QDialogButtonBox::accepted, this, &KNotePrintSelectedNotesDialog::accept);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &KNotePrintSelectedNotesDialog::reject);
    auto w = new QWidget;
    auto vbox = new QVBoxLayout;
    vbox->setContentsMargins(0, 0, 0, 0);
    w->setLayout(vbox);

    mListNotes = new QListWidget;
    mListNotes->setSelectionMode(QAbstractItemView::ExtendedSelection);
    vbox->addWidget(mListNotes);

    auto lay = new QHBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    vbox->addLayout(lay);
    auto lab = new QLabel(i18n("Printing theme:"));
    lay->addWidget(lab);
    mTheme = new KNotePrintSelectThemeComboBox;
    mTheme->loadThemes();
    lay->addWidget(mTheme);

    mUser1Button->setIcon(QIcon::fromTheme(QStringLiteral("document-print-preview")));
    mUser1Button->setText(i18n("Preview"));
    okButton->setIcon(QIcon::fromTheme(QStringLiteral("document-print")));
    okButton->setText(i18n("Print"));
    connect(mUser1Button, &QPushButton::clicked, this, &KNotePrintSelectedNotesDialog::slotPreview);
    connect(mListNotes, &QListWidget::itemSelectionChanged, this, &KNotePrintSelectedNotesDialog::slotSelectionChanged);
    mainLayout->addWidget(w);
    mainLayout->addWidget(mButtonBox);
    readConfig();
    slotSelectionChanged();
}

KNotePrintSelectedNotesDialog::~KNotePrintSelectedNotesDialog()
{
    writeConfig();
}

void KNotePrintSelectedNotesDialog::slotSelectionChanged()
{
    const bool hasSelection = (!mListNotes->selectedItems().isEmpty());
    mUser1Button->setEnabled(hasSelection);
    mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(hasSelection);
}

void KNotePrintSelectedNotesDialog::setNotes(const QHash<Akonadi::Item::Id, KNote *> &notes)
{
    mNotes = notes;
    QHashIterator<Akonadi::Item::Id, KNote *> i(notes);
    while (i.hasNext()) {
        i.next();
        auto item = new QListWidgetItem(mListNotes);
        item->setText(i.value()->name());
        item->setToolTip(i.value()->text());
        item->setData(AkonadiId, i.key());
    }
}

QList<KNotePrintObject *> KNotePrintSelectedNotesDialog::selectedNotes() const
{
    QList<KNotePrintObject *> lstPrintObj;
    const QList<QListWidgetItem *> lst = mListNotes->selectedItems();
    for (QListWidgetItem *item : lst) {
        Akonadi::Item::Id akonadiId = item->data(AkonadiId).toLongLong();
        if (akonadiId != -1) {
            auto obj = new KNotePrintObject(mNotes.value(akonadiId)->item());
            lstPrintObj.append(obj);
        }
    }
    return lstPrintObj;
}

QString KNotePrintSelectedNotesDialog::selectedTheme() const
{
    return mTheme->selectedTheme();
}

bool KNotePrintSelectedNotesDialog::preview() const
{
    return mPreview;
}

namespace
{
static const char myKNotePrintSelectedNotesDialogName[] = "KNotePrintSelectedNotesDialog";
}
void KNotePrintSelectedNotesDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(300, 200));
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1String(myKNotePrintSelectedNotesDialogName));
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void KNotePrintSelectedNotesDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1String(myKNotePrintSelectedNotesDialogName));
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}

void KNotePrintSelectedNotesDialog::slotPreview()
{
    mPreview = true;
    accept();
}

#include "moc_knoteprintselectednotesdialog.cpp"
