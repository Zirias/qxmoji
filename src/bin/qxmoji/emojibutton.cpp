#include "emojibutton.h"

#include "emoji.h"
#include "emojistring.h"

#include <QMouseEvent>

EmojiButton::EmojiButton(QWidget *parent) :
    QLabel(parent)
{
    hide();
}

const Emoji *EmojiButton::emoji() const
{
    return _emoji;
}

void EmojiButton::setEmoji(const Emoji *emoji)
{
    _emoji = emoji;
    if (emoji)
    {
	setText(Emoji_qstr(emoji));
	setToolTip(Emoji_name(emoji));
	show();
    }
    else hide();
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void EmojiButton::enterEvent(QEvent *ev)
#else
void EmojiButton::enterEvent(QEnterEvent *ev)
#endif
{
    (void) ev;
    setBackgroundRole(QPalette::Highlight);
    setAutoFillBackground(true);
    setCursor(Qt::PointingHandCursor);
}

void EmojiButton::leaveEvent(QEvent *ev)
{
    (void) ev;
    setBackgroundRole(QPalette::NoRole);
    setAutoFillBackground(false);
    unsetCursor();
}

void EmojiButton::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton && ev->buttons() == Qt::LeftButton)
    {
	emit clicked();
    }
}

#include "moc_emojibutton.cpp"
