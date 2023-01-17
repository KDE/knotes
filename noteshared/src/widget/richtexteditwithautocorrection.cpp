/*
  SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "richtexteditwithautocorrection.h"
#ifdef HAVE_KTEXTADDONS_TEXT_AUTOCORRECTION_SUPPORT
#include <TextAutoCorrection/AutoCorrection>
#else
#include <PimCommonAutoCorrection/AutoCorrection>
#endif

#include <QKeyEvent>

using namespace NoteShared;

class NoteShared::RichTextEditWithAutoCorrectionPrivate
{
public:
    RichTextEditWithAutoCorrectionPrivate()
#ifdef HAVE_KTEXTADDONS_TEXT_AUTOCORRECTION_SUPPORT
        : mAutoCorrection(new TextAutoCorrection::AutoCorrection())
#else
        : mAutoCorrection(new PimCommonAutoCorrection::AutoCorrection())
#endif
    {
    }

    ~RichTextEditWithAutoCorrectionPrivate()
    {
        if (mNeedToDelete) {
            delete mAutoCorrection;
        }
    }
#ifdef HAVE_KTEXTADDONS_TEXT_AUTOCORRECTION_SUPPORT
    TextAutoCorrection::AutoCorrection *mAutoCorrection = nullptr;
#else
    PimCommonAutoCorrection::AutoCorrection *mAutoCorrection = nullptr;
#endif
    bool mNeedToDelete = true;
};

RichTextEditWithAutoCorrection::RichTextEditWithAutoCorrection(QWidget *parent)
    : KPIMTextEdit::RichTextEditor(parent)
    , d(new NoteShared::RichTextEditWithAutoCorrectionPrivate)
{
}

RichTextEditWithAutoCorrection::~RichTextEditWithAutoCorrection() = default;
#ifdef HAVE_KTEXTADDONS_TEXT_AUTOCORRECTION_SUPPORT
void RichTextEditWithAutoCorrection::setAutocorrection(TextAutoCorrection::AutoCorrection *autocorrect)
#else
void RichTextEditWithAutoCorrection::setAutocorrection(PimCommonAutoCorrection::AutoCorrection *autocorrect)
#endif
{
    d->mNeedToDelete = false;
    delete d->mAutoCorrection;
    d->mAutoCorrection = autocorrect;
}

#ifdef HAVE_KTEXTADDONS_TEXT_AUTOCORRECTION_SUPPORT
TextAutoCorrection::AutoCorrection *RichTextEditWithAutoCorrection::autocorrection() const
#else
PimCommonAutoCorrection::AutoCorrection *RichTextEditWithAutoCorrection::autocorrection() const
#endif
{
    return d->mAutoCorrection;
}

void RichTextEditWithAutoCorrection::setAutocorrectionLanguage(const QString &language)
{
#ifdef HAVE_KTEXTADDONS_TEXT_AUTOCORRECTION_SUPPORT
    TextAutoCorrection::AutoCorrectionSettings *settings = d->mAutoCorrection->autoCorrectionSettings();
#else
    PimCommonAutoCorrection::AutoCorrectionSettings *settings = d->mAutoCorrection->autoCorrectionSettings();
#endif
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
