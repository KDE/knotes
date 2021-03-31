/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <KXMLGUIClient>
#include <QDialog>

class KNoteEdit;
class KToolBar;
class QLineEdit;
class QPushButton;
class KNoteEditDialog : public QDialog, virtual public KXMLGUIClient
{
    Q_OBJECT
public:
    explicit KNoteEditDialog(bool readOnly, QWidget *parent = nullptr);
    ~KNoteEditDialog() override;

    void setAcceptRichText(bool b);
    QString text() const;

    void setText(const QString &text);

    QString title() const;

    void setTitle(const QString &text);

    KNoteEdit *noteEdit() const;

    void setReadOnly(bool b);

    void setTabSize(int size);

    void setAutoIndentMode(bool newmode);

    void setTextFont(const QFont &font);

    void setColor(const QColor &fg, const QColor &bg);

    void setCursorPositionFromStart(int pos);

    int cursorPositionFromStart() const;
private Q_SLOTS:
    void slotTextChanged(const QString &text);

private:
    void init(bool readOnly);
    void readConfig();
    void writeConfig();
    QLineEdit *mTitleEdit = nullptr;
    KNoteEdit *mNoteEdit = nullptr;
    KToolBar *mTool = nullptr;
    QPushButton *mOkButton = nullptr;
};

