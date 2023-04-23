/*
  SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "config-noshared.h"
#include <KPIMTextEdit/RichTextEditor>
#ifdef HAVE_TEXT_AUTOCORRECTION_WIDGETS
namespace TextAutoCorrectionCore
#else
namespace TextAutoCorrection
#endif
{
class AutoCorrection;
}

namespace NoteShared
{
class RichTextEditWithAutoCorrectionPrivate;
/**
 * @brief The RichTextEditWithAutoCorrection class
 * @author Laurent Montel <montel@kde.org>
 */
class RichTextEditWithAutoCorrection : public KPIMTextEdit::RichTextEditor
{
    Q_OBJECT
public:
    explicit RichTextEditWithAutoCorrection(QWidget *parent = nullptr);
    ~RichTextEditWithAutoCorrection() override;

#ifdef HAVE_TEXT_AUTOCORRECTION_WIDGETS
    Q_REQUIRED_RESULT TextAutoCorrectionCore::AutoCorrection *autocorrection() const;
    void setAutocorrection(TextAutoCorrectionCore::AutoCorrection *autocorrect);
#else
    Q_REQUIRED_RESULT TextAutoCorrection::AutoCorrection *autocorrection() const;
    void setAutocorrection(TextAutoCorrection::AutoCorrection *autocorrect);
#endif

    void setAutocorrectionLanguage(const QString &language);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    std::unique_ptr<RichTextEditWithAutoCorrectionPrivate> const d;
};
}
