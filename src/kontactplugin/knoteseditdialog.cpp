/*
   SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knoteseditdialog.h"
#include "knoteedit.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <KToolBar>
#include <QLineEdit>

#include <KXMLGUIBuilder>
#include <KXMLGUIFactory>

#include <KConfigGroup>
#include <KSharedConfig>
#include <KWindowConfig>
#include <QAction>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>

KNoteEditDialog::KNoteEditDialog(bool readOnly, QWidget *parent)
    : QDialog(parent)
{
    init(readOnly);
}

void KNoteEditDialog::init(bool readOnly)
{
    setWindowTitle(readOnly ? i18nc("@title:window", "Show Popup Note") : i18nc("@title:window", "Edit Popup Note"));
    auto mainLayout = new QVBoxLayout(this);
    auto buttonBox = new QDialogButtonBox(readOnly ? QDialogButtonBox::Close : QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &KNoteEditDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &KNoteEditDialog::reject);
    if (readOnly) {
        buttonBox->button(QDialogButtonBox::Close)->setDefault(true);
    } else {
        buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
        mOkButton = buttonBox->button(QDialogButtonBox::Ok);
        mOkButton->setDefault(true);
        mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    }

    setModal(true);
    // this dialog is modal to prevent one from editing the same note twice
    // in two different windows
    setComponentName(QStringLiteral("knotes"), i18n("KNotes"));
    setXMLFile(QStringLiteral("knotesui.rc"));

    auto page = new QWidget(this);
    mainLayout->addWidget(page);
    mainLayout->addWidget(buttonBox);

    auto layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);

    auto hbl = new QHBoxLayout();
    layout->addLayout(hbl);
    auto label = new QLabel(page);
    label->setText(i18nc("@label popup note name", "Name:"));
    hbl->addWidget(label, 0);
    mTitleEdit = new QLineEdit(page);
    mTitleEdit->setClearButtonEnabled(!readOnly);
    mTitleEdit->setObjectName(QStringLiteral("name"));
    if (!readOnly) {
        connect(mTitleEdit, &QLineEdit::textChanged, this, &KNoteEditDialog::slotTextChanged);
    }
    hbl->addWidget(mTitleEdit, 1, Qt::AlignVCenter);

    // TODO customize it
    mNoteEdit = new KNoteEdit(actionCollection(), page);
    mNoteEdit->setFocus();

    KXMLGUIBuilder builder(page);
    KXMLGUIFactory factory(&builder, this);
    factory.addClient(this);

    mTool = static_cast<KToolBar *>(factory.container(QStringLiteral("note_tool"), this));
    layout->addWidget(mTool);
    layout->addWidget(mNoteEdit);

    actionCollection()->addAssociatedWidget(this);
    const auto lst = actionCollection()->actions();
    for (QAction *action : lst) {
        action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    }
    readConfig();
    setReadOnly(readOnly);
}

KNoteEditDialog::~KNoteEditDialog()
{
    writeConfig();
}

void KNoteEditDialog::setColor(const QColor &fg, const QColor &bg)
{
    mNoteEdit->setColor(fg, bg);
}

void KNoteEditDialog::setAcceptRichText(bool b)
{
    mNoteEdit->setAcceptRichText(b);
    mTool->setVisible(b);
}

void KNoteEditDialog::setReadOnly(bool b)
{
    mNoteEdit->setEnabled(!b);
    mTool->setEnabled(!b);
    mTitleEdit->setEnabled(!b);
}

namespace
{
static const char myKNoteEditDialogName[] = "KNoteEditDialog";
}
void KNoteEditDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(300, 200));
    KConfigGroup group(KSharedConfig::openStateConfig(), myKNoteEditDialogName);
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void KNoteEditDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myKNoteEditDialogName);
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}

QString KNoteEditDialog::text() const
{
    return mNoteEdit->text();
}

void KNoteEditDialog::setText(const QString &text)
{
    mNoteEdit->setText(text);
}

QString KNoteEditDialog::title() const
{
    return mTitleEdit->text();
}

void KNoteEditDialog::setTitle(const QString &text)
{
    mTitleEdit->setText(text);
    if (mTitleEdit->isEnabled() && mOkButton) {
        mOkButton->setEnabled(!text.trimmed().isEmpty());
    }
}

KNoteEdit *KNoteEditDialog::noteEdit() const
{
    return mNoteEdit;
}

void KNoteEditDialog::slotTextChanged(const QString &text)
{
    if (mOkButton) {
        mOkButton->setEnabled(!text.trimmed().isEmpty());
    }
}

void KNoteEditDialog::setTabSize(int size)
{
    mNoteEdit->setTabStop(size);
}

void KNoteEditDialog::setAutoIndentMode(bool newmode)
{
    mNoteEdit->setAutoIndentMode(newmode);
}

void KNoteEditDialog::setTextFont(const QFont &font)
{
    mNoteEdit->setTextFont(font);
}

void KNoteEditDialog::setCursorPositionFromStart(int pos)
{
    mNoteEdit->setCursorPositionFromStart(pos);
}

int KNoteEditDialog::cursorPositionFromStart() const
{
    return mNoteEdit->cursorPositionFromStart();
}

#include "moc_knoteseditdialog.cpp"
