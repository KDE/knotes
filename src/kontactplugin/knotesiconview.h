/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "knotes_part.h"
#include <QListWidget>
#include <QMultiHash>
class KNoteDisplaySettings;
class KNotesIconViewItem;
class KNotesIconView : public QListWidget
{
    Q_OBJECT
public:
    explicit KNotesIconView(KNotesPart *part, QWidget *parent);
    ~KNotesIconView() override;

    void addNote(const Akonadi::Item &item);

    KNotesIconViewItem *iconView(Akonadi::Item::Id id) const;
    QHash<Akonadi::Item::Id, KNotesIconViewItem *> noteList() const;

protected:
    void mousePressEvent(QMouseEvent *) override;

    bool event(QEvent *e) override;

private:
    KNotesPart *m_part = nullptr;
    QHash<Akonadi::Item::Id, KNotesIconViewItem *> mNoteList;
};

class KNotesIconViewItem : public QObject, public QListWidgetItem
{
    Q_OBJECT
public:
    KNotesIconViewItem(const Akonadi::Item &item, QListWidget *parent);
    ~KNotesIconViewItem();

    bool readOnly() const;
    void setReadOnly(bool b, bool save = true);

    void setIconText(const QString &text, bool save = true);
    QString realName() const;

    int tabSize() const;
    bool autoIndent() const;
    QFont textFont() const;
    bool isRichText() const;
    QString description() const;
    void setDescription(const QString &);
    KNoteDisplaySettings *displayAttribute() const;
    Akonadi::Item item();

    void setChangeItem(const Akonadi::Item &item, const QSet<QByteArray> &set);
    void saveNoteContent(const QString &subject = QString(), const QString &description = QString(), int position = -1);
    void updateSettings();
    void setChangeIconTextAndDescription(const QString &iconText, const QString &description, int position);
    QColor textBackgroundColor() const;
    QColor textForegroundColor() const;

    int cursorPositionFromStart() const;

private Q_SLOTS:
    void slotNoteSaved(KJob *job);

private:
    void prepare();
    void setDisplayDefaultValue();
    QPixmap mDefaultPixmap;

    Akonadi::Item mItem;
    KNoteDisplaySettings *mDisplayAttribute = nullptr;
    bool mReadOnly;
};

