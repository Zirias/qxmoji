#include "emojibutton.h"

#include "emoji.h"
#include "emojistring.h"

#include <QMouseEvent>

EmojiButton::EmojiButton(QWidget *parent) :
    QLabel(parent)
{
    hide();
    setBackgroundRole(QPalette::Highlight);
    setCursor(Qt::PointingHandCursor);
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
	setToolTip(Emoji_localname(emoji));
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
    setAutoFillBackground(true);
}

void EmojiButton::leaveEvent(QEvent *ev)
{
    (void) ev;
    setAutoFillBackground(false);
}

void EmojiButton::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton && ev->buttons() == Qt::LeftButton)
    {
	emit clicked();
    }
}

#include "moc_emojibutton.cpp"
