#include "qxmojiwin.h"

#include "emoji.h"
#include "emojibutton.h"

class QXmojiWinPrivate {
    Q_DISABLE_COPY(QXmojiWinPrivate)
    Q_DECLARE_PUBLIC(QXmojiWin)
    QXmojiWin *const q_ptr;

    EmojiButton button;

    QXmojiWinPrivate(QXmojiWin *);
};

QXmojiWinPrivate::QXmojiWinPrivate(QXmojiWin *win) :
    q_ptr(win),
    button(win, EmojiGroup_emojis(emojigroups))
{
    button.show();
}

QXmojiWin::QXmojiWin() :
    QWidget(nullptr, Qt::WindowDoesNotAcceptFocus),
    d_ptr(new QXmojiWinPrivate(this))
{
    setAttribute(Qt::WA_AlwaysShowToolTips);
}

QXmojiWin::~QXmojiWin() {}
