/*
  SPDX-FileCopyrightText: 2013-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KPIMTextEdit/RichTextEditor>
namespace PimCommonAutoCorrection
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

    Q_REQUIRED_RESULT PimCommonAutoCorrection::AutoCorrection *autocorrection() const;

    void setAutocorrection(PimCommonAutoCorrection::AutoCorrection *autocorrect);

    void setAutocorrectionLanguage(const QString &language);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    std::unique_ptr<RichTextEditWithAutoCorrectionPrivate> const d;
};
}
