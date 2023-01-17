/*
  SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KPIMTextEdit/RichTextEditor>
#ifdef HAVE_KTEXTADDONS_TEXT_AUTOCORRECTION_SUPPORT
namespace TextAutoCorrection
#else
namespace PimCommonAutoCorrection
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

#ifdef HAVE_KTEXTADDONS_TEXT_AUTOCORRECTION_SUPPORT
    Q_REQUIRED_RESULT TextAutoCorrection::AutoCorrection *autocorrection() const;
    void setAutocorrection(TextAutoCorrection::AutoCorrection *autocorrect);
#else
    Q_REQUIRED_RESULT PimCommonAutoCorrection::AutoCorrection *autocorrection() const;
    void setAutocorrection(PimCommonAutoCorrection::AutoCorrection *autocorrect);
#endif

    void setAutocorrectionLanguage(const QString &language);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    std::unique_ptr<RichTextEditWithAutoCorrectionPrivate> const d;
};
}
