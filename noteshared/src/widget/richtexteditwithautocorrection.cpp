/*
  SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "richtexteditwithautocorrection.h"
#include <TextAutoCorrection/AutoCorrection>

#include <QKeyEvent>

using namespace NoteShared;

class NoteShared::RichTextEditWithAutoCorrectionPrivate
{
public:
    RichTextEditWithAutoCorrectionPrivate()
        : mAutoCorrection(new TextAutoCorrection::AutoCorrection())
    {
    }

    ~RichTextEditWithAutoCorrectionPrivate()
    {
        if (mNeedToDelete) {
            delete mAutoCorrection;
        }
    }
    TextAutoCorrection::AutoCorrection *mAutoCorrection = nullptr;
    bool mNeedToDelete = true;
};

RichTextEditWithAutoCorrection::RichTextEditWithAutoCorrection(QWidget *parent)
    : KPIMTextEdit::RichTextEditor(parent)
    , d(new NoteShared::RichTextEditWithAutoCorrectionPrivate)
{
}

RichTextEditWithAutoCorrection::~RichTextEditWithAutoCorrection() = default;
void RichTextEditWithAutoCorrection::setAutocorrection(TextAutoCorrection::AutoCorrection *autocorrect)
{
    d->mNeedToDelete = false;
    delete d->mAutoCorrection;
    d->mAutoCorrection = autocorrect;
}

TextAutoCorrection::AutoCorrection *RichTextEditWithAutoCorrection::autocorrection() const
{
    return d->mAutoCorrection;
}

void RichTextEditWithAutoCorrection::setAutocorrectionLanguage(const QString &language)
{
    TextAutoCorrection::AutoCorrectionSettings *settings = d->mAutoCorrection->autoCorrectionSettings();
    settings->setLanguage(language);
    d->mAutoCorrection->setAutoCorrectionSettings(settings);
}

static bool isSpecial(const QTextCharFormat &charFormat)
{
    return charFormat.isFrameFormat() || charFormat.isImageFormat() || charFormat.isListFormat() || charFormat.isTableFormat()
        || charFormat.isTableCellFormat();
}

void RichTextEditWithAutoCorrection::keyPressEvent(QKeyEvent *e)
{
    if (d->mAutoCorrection && d->mAutoCorrection->autoCorrectionSettings()->isEnabledAutoCorrection()) {
        if ((e->key() == Qt::Key_Space) || (e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return)) {
            if (!textCursor().hasSelection()) {
                const QTextCharFormat initialTextFormat = textCursor().charFormat();
                const bool richText = acceptRichText();
                int position = textCursor().position();
                const bool addSpace = d->mAutoCorrection->autocorrect(richText, *document(), position);
                QTextCursor cur = textCursor();
                cur.setPosition(position);
                const bool spacePressed = (e->key() == Qt::Key_Space);
                const QChar insertChar = spacePressed ? QLatin1Char(' ') : QLatin1Char('\n');
                if (richText && !isSpecial(initialTextFormat)) {
                    if (addSpace || !spacePressed) {
                        cur.insertText(insertChar, initialTextFormat);
                    }
                } else {
                    if (addSpace || !spacePressed) {
                        cur.insertText(insertChar);
                    }
                }
                setTextCursor(cur);
                return;
            }
        }
    }
    KPIMTextEdit::RichTextEditor::keyPressEvent(e);
}
