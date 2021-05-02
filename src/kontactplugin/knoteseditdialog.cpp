/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "knoteseditdialog.h"
#include "knoteedit.h"

#include <KActionCollection>
#include <KLineEdit>
#include <KLocalizedString>
#include <KToolBar>

#include <KXMLGUIBuilder>
#include <KXMLGUIFactory>

#include "knotes_kontact_plugin_debug.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <QAction>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

KNoteEditDialog::KNoteEditDialog(bool readOnly, QWidget *parent)
    : QDialog(parent)
    , mOkButton(nullptr)
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
    mTitleEdit = new KLineEdit(page);
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

void KNoteEditDialog::readConfig()
{
    KConfigGroup grp(KSharedConfig::openStateConfig(), "KNoteEditDialog");
    const QSize size = grp.readEntry("Size", QSize(300, 200));
    if (size.isValid()) {
        resize(size);
    }
}

void KNoteEditDialog::writeConfig()
{
    KConfigGroup grp(KSharedConfig::openStateConfig(), "KNoteEditDialog");
    grp.writeEntry("Size", size());
    grp.sync();
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
