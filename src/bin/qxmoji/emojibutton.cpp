#include "emojibutton.h"

#include <QFontDatabase>

#include "emoji.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define dbfont(f,s,p) QFontDatabase::font((f),(s),(p))
#define sysfont QFontDatabase::systemFont(QFontDatabase::GeneralFont)
#else
#define dbfont(f,s,p) qfontdatabase.font((f),(s),(p))
#define sysfont qfontdatabase.systemFont(QFontDatabase::GeneralFont)
#endif

EmojiButton::EmojiButton(QWidget *parent, const Emoji *emoji) :
    QLabel(parent), emoji(emoji)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QFontDatabase qfontdatabase;
#endif
    int pointsize = sysfont.pointSize();
    if (pointsize > 0) pointsize += pointsize / 2;
    else pointsize = 14;
    QFont font = dbfont("Noto Color Emoji", QString(), pointsize);
    if (font.family() != "Noto Color Emoji")
    {
	font = dbfont("Noto Emoji", QString(), pointsize);
    }
    setFont(font);
    setText(QString::fromUcs4(Emoji_codepoints(emoji)));
    setToolTip(Emoji_name(emoji));
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);
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
    setBackgroundRole(QPalette::Window);
    unsetCursor();
}

void EmojiButton::mousePressEvent(QMouseEvent *ev)
{
    (void) ev;
    emit clicked(emoji);
}

