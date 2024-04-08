#include "emojibutton.h"

#include "emoji.h"

EmojiButton::EmojiButton(QWidget *parent, const Emoji *emoji) :
    QLabel(parent), _emoji(emoji)
{
    setText("xx");
}

const Emoji *EmojiButton::emoji() const
{
    return _emoji;
}

void EmojiButton::showEvent(QShowEvent *ev)
{
    if (toolTip().isEmpty())
    {
	setText(QString::fromUcs4(Emoji_codepoints(_emoji)));
	setToolTip(Emoji_name(_emoji));
	setAutoFillBackground(true);
    }
    QWidget::showEvent(ev);
}

void EmojiButton::enterEvent(QEvent *ev)
{
    (void) ev;
    setBackgroundRole(QPalette::Highlight);
    setCursor(Qt::PointingHandCursor);
}

void EmojiButton::leaveEvent(QEvent *ev)
{
    (void) ev;
    setBackgroundRole(QPalette::NoRole);
    unsetCursor();
}

void EmojiButton::mousePressEvent(QMouseEvent *ev)
{
    (void) ev;
    emit clicked();
}

