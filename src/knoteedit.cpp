/*******************************************************************
 KNotes -- Notes for the KDE project

 SPDX-FileCopyrightText: 1997-2013 The KNotes Developers

 SPDX-License-Identifier: GPL-2.0-or-later
*******************************************************************/

#include "knoteedit.h"
#include "editor/noteeditorutils.h"
#include "notes/knote.h"

#include <TextUtils/ConvertText>

#include <PimCommon/KActionMenuChangeCase>

#include <KActionCollection>
#include <KFontAction>
#include <KFontSizeAction>
#include <KLocalizedString>
#include <KToggleAction>

#include <QActionGroup>
#include <QColorDialog>
#include <QIcon>
#include <QMenu>

static const short ICON_SIZE = 10;

KNoteEdit::KNoteEdit(KActionCollection *actions, QWidget *parent)
    : KTextEdit(parent)
    , m_note(nullptr)
    , m_actions(actions)
{
    setAcceptDrops(true);
    setWordWrapMode(QTextOption::WordWrap);
    setLineWrapMode(WidgetWidth);
    if (acceptRichText()) {
        setAutoFormatting(AutoAll);
    } else {
        setAutoFormatting(AutoNone);
    }

    // create the actions modifying the text format
    m_textBold = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-text-bold")), i18n("Bold"), this);
    actions->addAction(QStringLiteral("format_bold"), m_textBold);
    actions->setDefaultShortcut(m_textBold, QKeySequence(Qt::CTRL | Qt::Key_B));
    m_textItalic = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-text-italic")), i18n("Italic"), this);
    actions->addAction(QStringLiteral("format_italic"), m_textItalic);
    actions->setDefaultShortcut(m_textItalic, QKeySequence(Qt::CTRL | Qt::Key_I));
    m_textUnderline = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-text-underline")), i18n("Underline"), this);
    actions->addAction(QStringLiteral("format_underline"), m_textUnderline);
    actions->setDefaultShortcut(m_textUnderline, QKeySequence(Qt::CTRL | Qt::Key_U));
    m_textStrikeOut = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-text-strikethrough")), i18n("Strike Out"), this);
    actions->addAction(QStringLiteral("format_strikeout"), m_textStrikeOut);
    actions->setDefaultShortcut(m_textStrikeOut, QKeySequence(Qt::CTRL | Qt::Key_S));

    connect(m_textBold, &KToggleAction::toggled, this, &KNoteEdit::textBold);
    connect(m_textItalic, &KToggleAction::toggled, this, &KNoteEdit::setFontItalic);
    connect(m_textUnderline, &KToggleAction::toggled, this, &KNoteEdit::setFontUnderline);
    connect(m_textStrikeOut, &KToggleAction::toggled, this, &KNoteEdit::textStrikeOut);

    m_textAlignLeft = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-justify-left")), i18n("Align Left"), this);
    actions->addAction(QStringLiteral("format_alignleft"), m_textAlignLeft);
    connect(m_textAlignLeft, &KToggleAction::triggered, this, &KNoteEdit::textAlignLeft);
    actions->setDefaultShortcut(m_textAlignLeft, QKeySequence(Qt::ALT | Qt::Key_L));
    m_textAlignLeft->setChecked(true); // just a dummy, will be updated later
    m_textAlignCenter = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-justify-center")), i18n("Align Center"), this);
    actions->addAction(QStringLiteral("format_aligncenter"), m_textAlignCenter);
    connect(m_textAlignCenter, &KToggleAction::triggered, this, &KNoteEdit::textAlignCenter);
    actions->setDefaultShortcut(m_textAlignCenter, QKeySequence(Qt::ALT | Qt::Key_C));
    m_textAlignRight = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-justify-right")), i18n("Align Right"), this);
    actions->addAction(QStringLiteral("format_alignright"), m_textAlignRight);
    connect(m_textAlignRight, &KToggleAction::triggered, this, &KNoteEdit::textAlignRight);
    actions->setDefaultShortcut(m_textAlignRight, QKeySequence(Qt::ALT | Qt::Key_R));
    m_textAlignBlock = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-justify-fill")), i18n("Align Block"), this);
    actions->addAction(QStringLiteral("format_alignblock"), m_textAlignBlock);
    connect(m_textAlignBlock, &KToggleAction::triggered, this, &KNoteEdit::textAlignBlock);
    actions->setDefaultShortcut(m_textAlignBlock, QKeySequence(Qt::ALT | Qt::Key_B));

    auto group = new QActionGroup(this);
    group->addAction(m_textAlignLeft);
    group->addAction(m_textAlignCenter);
    group->addAction(m_textAlignRight);
    group->addAction(m_textAlignBlock);

    m_textList = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-list-ordered")), i18n("List"), this);
    actions->addAction(QStringLiteral("format_list"), m_textList);
    connect(m_textList, &KToggleAction::triggered, this, &KNoteEdit::textList);

    m_textSuper = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-text-superscript")), i18n("Superscript"), this);
    actions->addAction(QStringLiteral("format_super"), m_textSuper);
    connect(m_textSuper, &KToggleAction::triggered, this, &KNoteEdit::textSuperScript);
    m_textSub = new KToggleAction(QIcon::fromTheme(QStringLiteral("format-text-subscript")), i18n("Subscript"), this);
    actions->addAction(QStringLiteral("format_sub"), m_textSub);
    connect(m_textSub, &KToggleAction::triggered, this, &KNoteEdit::textSubScript);

    m_textIncreaseIndent = new QAction(QIcon::fromTheme(QStringLiteral("format-indent-more")), i18n("Increase Indent"), this);
    actions->addAction(QStringLiteral("format_increaseindent"), m_textIncreaseIndent);
    actions->setDefaultShortcut(m_textIncreaseIndent, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_I));
    connect(m_textIncreaseIndent, &QAction::triggered, this, &KNoteEdit::textIncreaseIndent);

    m_textDecreaseIndent = new QAction(QIcon::fromTheme(QStringLiteral("format-indent-less")), i18n("Decrease Indent"), this);
    actions->addAction(QStringLiteral("format_decreaseindent"), m_textDecreaseIndent);
    actions->setDefaultShortcut(m_textDecreaseIndent, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_D));
    connect(m_textDecreaseIndent, &QAction::triggered, this, &KNoteEdit::textDecreaseIndent);

    group = new QActionGroup(this);
    group->addAction(m_textIncreaseIndent);
    group->addAction(m_textDecreaseIndent);

    QPixmap pix(ICON_SIZE, ICON_SIZE);
    pix.fill(Qt::black); // just a dummy, gets updated before widget is shown
    m_textColor = new QAction(i18n("Text Color..."), this);
    actions->addAction(QStringLiteral("format_color"), m_textColor);
    m_textColor->setIcon(pix);
    connect(m_textColor, &QAction::triggered, this, &KNoteEdit::slotTextColor);

    auto act = new QAction(QIcon::fromTheme(QStringLiteral("format-fill-color")), i18n("Text Background Color..."), this);
    actions->addAction(QStringLiteral("text_background_color"), act);
    connect(act, &QAction::triggered, this, &KNoteEdit::slotTextBackgroundColor);

    m_textFont = new KFontAction(i18n("Text Font"), this);
    actions->addAction(QStringLiteral("format_font"), m_textFont);
    connect(m_textFont, &KFontAction::textTriggered, this, &KNoteEdit::setFontFamily);
    m_textSize = new KFontSizeAction(i18n("Text Size"), this);
    actions->addAction(QStringLiteral("format_size"), m_textSize);
    connect(m_textSize, &KFontSizeAction::fontSizeChanged, this, &KNoteEdit::setTextFontSize);

    mChangeCaseActionMenu = new PimCommon::KActionMenuChangeCase(this);
    mChangeCaseActionMenu->appendInActionCollection(actions);
    connect(mChangeCaseActionMenu, &PimCommon::KActionMenuChangeCase::upperCase, this, &KNoteEdit::slotUpperCase);
    connect(mChangeCaseActionMenu, &PimCommon::KActionMenuChangeCase::lowerCase, this, &KNoteEdit::slotLowerCase);
    connect(mChangeCaseActionMenu, &PimCommon::KActionMenuChangeCase::sentenceCase, this, &KNoteEdit::slotSentenceCase);
    connect(mChangeCaseActionMenu, &PimCommon::KActionMenuChangeCase::reverseCase, this, &KNoteEdit::slotReverseCase);

    auto action = new QAction(QIcon::fromTheme(QStringLiteral("knotes_date")), i18n("Insert Date"), this);
    actions->addAction(QStringLiteral("insert_date"), action);
    connect(action, &QAction::triggered, this, &KNoteEdit::slotInsertDate);

    action = new QAction(QIcon::fromTheme(QStringLiteral("checkmark")), i18n("Insert Checkmark"), this);
    actions->addAction(QStringLiteral("insert_checkmark"), action);
    connect(action, &QAction::triggered, this, &KNoteEdit::slotInsertCheckMark);

    // QTextEdit connections
    connect(this, &KNoteEdit::currentCharFormatChanged, this, &KNoteEdit::slotCurrentCharFormatChanged);
    connect(this, &KNoteEdit::cursorPositionChanged, this, &KNoteEdit::slotCursorPositionChanged);
    slotCurrentCharFormatChanged(currentCharFormat());
    slotCursorPositionChanged();
}

KNoteEdit::~KNoteEdit() = default;

void KNoteEdit::setColor(const QColor &fg, const QColor &bg)
{
    mDefaultBackgroundColor = bg;
    mDefaultForegroundColor = fg;

    QPalette p = palette();

    // better: from light(150) to light(100) to light(75)
    // QLinearGradient g( width()/2, 0, width()/2, height() );
    // g.setColorAt( 0, bg );
    // g.setColorAt( 1, bg.darker(150) );

    p.setColor(QPalette::Window, bg);
    // p.setBrush( QPalette::Window,     g );
    p.setColor(QPalette::Base, bg);
    // p.setBrush( QPalette::Base,       g );

    p.setColor(QPalette::WindowText, fg);
    p.setColor(QPalette::Text, fg);

    p.setColor(QPalette::Button, bg.darker(116));
    p.setColor(QPalette::ButtonText, fg);

    // p.setColor( QPalette::Highlight,  bg );
    // p.setColor( QPalette::HighlightedText, fg );

    // order: Light, Midlight, Button, Mid, Dark, Shadow

    // the shadow
    p.setColor(QPalette::Light, bg.lighter(180));
    p.setColor(QPalette::Midlight, bg.lighter(150));
    p.setColor(QPalette::Mid, bg.lighter(150));
    p.setColor(QPalette::Dark, bg.darker(108));
    p.setColor(QPalette::Shadow, bg.darker(116));

    setPalette(p);

    setTextColor(fg);
}

void KNoteEdit::setNote(KNote *_note)
{
    m_note = _note;
}

void KNoteEdit::slotReverseCase()
{
    QTextCursor cursor = textCursor();
    TextUtils::ConvertText::reverseCase(cursor);
}

void KNoteEdit::slotSentenceCase()
{
    QTextCursor cursor = textCursor();
    TextUtils::ConvertText::sentenceCase(cursor);
}

void KNoteEdit::slotUpperCase()
{
    QTextCursor cursor = textCursor();
    TextUtils::ConvertText::upperCase(cursor);
}

void KNoteEdit::slotLowerCase()
{
    QTextCursor cursor = textCursor();
    TextUtils::ConvertText::lowerCase(cursor);
}

QMenu *KNoteEdit::mousePopupMenu()
{
    QMenu *popup = KTextEdit::mousePopupMenu();
    if (popup) {
        QTextCursor cursor = textCursor();
        if (!isReadOnly()) {
            if (cursor.hasSelection()) {
                popup->addSeparator();
                popup->addAction(mChangeCaseActionMenu);
            }
            popup->addSeparator();
            QAction *act = m_actions->action(QStringLiteral("insert_date"));
            popup->addAction(act);
            popup->addSeparator();
            act = m_actions->action(QStringLiteral("insert_checkmark"));
            popup->addAction(act);
        }
    }
    return popup;
}

void KNoteEdit::setText(const QString &text)
{
    if (acceptRichText() && Qt::mightBeRichText(text)) {
        setHtml(text);
    } else {
        setPlainText(text);
    }
}

QString KNoteEdit::text() const
{
    if (acceptRichText()) {
        return toHtml();
    } else {
        return toPlainText();
    }
}

void KNoteEdit::setTextFont(const QFont &font)
{
    setCurrentFont(font);

    // make this font default so that if user deletes note content
    // font is remembered
    document()->setDefaultFont(font);
}

void KNoteEdit::setTextFontSize(int size)
{
    setFontPointSize(size);
}

void KNoteEdit::setTabStop(int tabs)
{
    QFontMetrics fm(font());
    setTabStopDistance(fm.boundingRect(QLatin1Char('x')).width() * tabs);
}

void KNoteEdit::setAutoIndentMode(bool newmode)
{
    m_autoIndentMode = newmode;
}

/** public slots **/

void KNoteEdit::setRichText(bool f)
{
    if (f == acceptRichText()) {
        return;
    }

    setAcceptRichText(f);

    if (f) {
        setAutoFormatting(AutoAll);
    } else {
        setAutoFormatting(AutoNone);
    }

    const QString t = toPlainText();
    if (f) {
        // if the note contains html source try to render it
        if (Qt::mightBeRichText(t)) {
            setHtml(t);
        } else {
            setPlainText(t);
        }

        enableRichTextActions(true);
    } else {
        setPlainText(t);
        enableRichTextActions(false);
    }
}

void KNoteEdit::textBold(bool b)
{
    if (!acceptRichText()) {
        return;
    }

    QTextCharFormat f;
    f.setFontWeight(b ? QFont::Bold : QFont::Normal);
    mergeCurrentCharFormat(f);
}

void KNoteEdit::textStrikeOut(bool s)
{
    if (!acceptRichText()) {
        return;
    }

    QTextCharFormat f;
    f.setFontStrikeOut(s);
    mergeCurrentCharFormat(f);
}

void KNoteEdit::slotTextColor()
{
    if (!acceptRichText()) {
        return;
    }

    if (m_note) {
        m_note->setBlockSave(true);
    }
    QColor c = QColorDialog::getColor(textColor(), this);
    if (c.isValid()) {
        setTextColor(c);
    }
    if (m_note) {
        m_note->setBlockSave(false);
    }
}

void KNoteEdit::slotTextBackgroundColor()
{
    if (!acceptRichText()) {
        return;
    }

    if (m_note) {
        m_note->setBlockSave(true);
    }
    const QColor c = QColorDialog::getColor(textBackgroundColor(), this);
    if (c.isValid()) {
        setTextBackgroundColor(c);
    }
    if (m_note) {
        m_note->setBlockSave(false);
    }
}

void KNoteEdit::textAlignLeft()
{
    if (!acceptRichText()) {
        return;
    }
    setAlignment(Qt::AlignLeft);
    m_textAlignLeft->setChecked(true);
}

void KNoteEdit::textAlignCenter()
{
    if (!acceptRichText()) {
        return;
    }
    setAlignment(Qt::AlignCenter);
    m_textAlignCenter->setChecked(true);
}

void KNoteEdit::textAlignRight()
{
    if (!acceptRichText()) {
        return;
    }
    setAlignment(Qt::AlignRight);
    m_textAlignRight->setChecked(true);
}

void KNoteEdit::textAlignBlock()
{
    if (!acceptRichText()) {
        return;
    }
    setAlignment(Qt::AlignJustify);
    m_textAlignBlock->setChecked(true);
}

void KNoteEdit::textList()
{
    if (!acceptRichText()) {
        return;
    }
    QTextCursor c = textCursor();
    c.beginEditBlock();

    if (m_textList->isChecked()) {
        QTextListFormat lf;
        QTextBlockFormat bf = c.blockFormat();

        lf.setIndent(bf.indent() + 1);
        bf.setIndent(0);

        lf.setStyle(QTextListFormat::ListDisc);

        c.setBlockFormat(bf);
        c.createList(lf);
    } else {
        QTextBlockFormat bf;
        bf.setObjectIndex(-1);
        c.setBlockFormat(bf);
    }

    c.endEditBlock();
}

void KNoteEdit::textSuperScript()
{
    if (!acceptRichText()) {
        return;
    }
    QTextCharFormat f;
    if (m_textSuper->isChecked()) {
        if (m_textSub->isChecked()) {
            m_textSub->setChecked(false);
        }
        f.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    } else {
        f.setVerticalAlignment(QTextCharFormat::AlignNormal);
    }
    mergeCurrentCharFormat(f);
}

void KNoteEdit::textSubScript()
{
    if (!acceptRichText()) {
        return;
    }
    QTextCharFormat f;
    if (m_textSub->isChecked()) {
        if (m_textSuper->isChecked()) {
            m_textSuper->setChecked(false);
        }
        f.setVerticalAlignment(QTextCharFormat::AlignSubScript);
    } else {
        f.setVerticalAlignment(QTextCharFormat::AlignNormal);
    }
    mergeCurrentCharFormat(f);
}

void KNoteEdit::textIncreaseIndent()
{
    if (!acceptRichText()) {
        return;
    }
    QTextBlockFormat f = textCursor().blockFormat();
    f.setIndent(f.indent() + 1);
    textCursor().setBlockFormat(f);
}

void KNoteEdit::textDecreaseIndent()
{
    if (!acceptRichText()) {
        return;
    }
    QTextBlockFormat f = textCursor().blockFormat();
    short int curIndent = f.indent();

    if (curIndent > 0) {
        f.setIndent(curIndent - 1);
    }
    textCursor().setBlockFormat(f);
}

/** protected methods **/

void KNoteEdit::keyPressEvent(QKeyEvent *e)
{
    KTextEdit::keyPressEvent(e);

    if (m_autoIndentMode && ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))) {
        autoIndent();
    }
}

void KNoteEdit::focusInEvent(QFocusEvent *e)
{
    KTextEdit::focusInEvent(e);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void KNoteEdit::focusOutEvent(QFocusEvent *e)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    KTextEdit::focusOutEvent(e);
}

/** private slots **/

void KNoteEdit::slotCurrentCharFormatChanged(const QTextCharFormat &f)
{
    if (!acceptRichText()) {
        return;
    }

    // font changes
    const QStringList families = f.fontFamilies().toStringList();
    const auto fontFamily = families.isEmpty() ? QString() : families.constFirst();
    m_textFont->setFont(fontFamily);
    m_textSize->setFontSize((f.fontPointSize() > 0) ? (int)f.fontPointSize() : 10);

    m_textBold->setChecked(f.font().bold());
    m_textItalic->setChecked(f.fontItalic());
    m_textUnderline->setChecked(f.fontUnderline());
    m_textStrikeOut->setChecked(f.fontStrikeOut());

    // color changes
    QPixmap pix(ICON_SIZE, ICON_SIZE);
    pix.fill(f.foreground().color());
    m_textColor->QAction::setIcon(pix);

    // vertical alignment changes
    QTextCharFormat::VerticalAlignment va = f.verticalAlignment();
    if (va == QTextCharFormat::AlignNormal) {
        m_textSuper->setChecked(false);
        m_textSub->setChecked(false);
    } else if (va == QTextCharFormat::AlignSuperScript) {
        m_textSuper->setChecked(true);
    } else if (va == QTextCharFormat::AlignSubScript) {
        m_textSub->setChecked(true);
    }
}

void KNoteEdit::slotCursorPositionChanged()
{
    if (!acceptRichText()) {
        return;
    }
    // alignment changes
    const Qt::Alignment a = alignment();
    if (a & Qt::AlignLeft) {
        m_textAlignLeft->setChecked(true);
    } else if (a & Qt::AlignHCenter) {
        m_textAlignCenter->setChecked(true);
    } else if (a & Qt::AlignRight) {
        m_textAlignRight->setChecked(true);
    } else if (a & Qt::AlignJustify) {
        m_textAlignBlock->setChecked(true);
    }
}

/** private methods **/

void KNoteEdit::autoIndent()
{
    QTextCursor c = textCursor();
    QTextBlock b = c.block();

    QString string;
    while ((b.previous().length() > 0) && string.trimmed().isEmpty()) {
        b = b.previous();
        string = b.text();
    }

    if (string.trimmed().isEmpty()) {
        return;
    }

    // This routine returns the whitespace before the first non white space
    // character in string.
    // It is assumed that string contains at least one non whitespace character
    // ie \n \r \t \v \f and space
    QString indentString;

    const int len = string.length();
    int i = 0;
    while (i < len && string.at(i).isSpace()) {
        indentString += string.at(i++);
    }

    if (!indentString.isEmpty()) {
        c.insertText(indentString);
    }
}

void KNoteEdit::enableRichTextActions(bool enabled)
{
    m_textColor->setEnabled(enabled);
    m_textFont->setEnabled(enabled);
    m_textSize->setEnabled(enabled);

    m_textBold->setEnabled(enabled);
    m_textItalic->setEnabled(enabled);
    m_textUnderline->setEnabled(enabled);
    m_textStrikeOut->setEnabled(enabled);

    m_textAlignLeft->setEnabled(enabled);
    m_textAlignCenter->setEnabled(enabled);
    m_textAlignRight->setEnabled(enabled);
    m_textAlignBlock->setEnabled(enabled);

    m_textList->setEnabled(enabled);
    m_textSuper->setEnabled(enabled);
    m_textSub->setEnabled(enabled);

    m_textIncreaseIndent->setEnabled(enabled);
    m_textDecreaseIndent->setEnabled(enabled);
}

void KNoteEdit::slotInsertDate()
{
    NoteShared::NoteEditorUtils noteEditorUtils;
    noteEditorUtils.insertDate(this);
}

void KNoteEdit::slotInsertCheckMark()
{
    QTextCursor cursor = textCursor();
    NoteShared::NoteEditorUtils noteEditorUtils;
    noteEditorUtils.addCheckmark(cursor);
}

void KNoteEdit::setCursorPositionFromStart(int pos)
{
    if (pos > 0) {
        QTextCursor cursor = textCursor();
        // Fix html pos cursor
        cursor.setPosition(qMin(pos, cursor.document()->characterCount() - 1));
        setTextCursor(cursor);
        ensureCursorVisible();
    }
}

int KNoteEdit::cursorPositionFromStart() const
{
    return textCursor().position();
}

#include "moc_knoteedit.cpp"
