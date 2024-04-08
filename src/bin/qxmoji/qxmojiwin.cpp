#include "qxmojiwin.h"

#include "emoji.h"
#include "emojibutton.h"
#include "flowlayout.h"

#include <QFontDatabase>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTabBar>
#include <QTabWidget>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define dbfont(f,s,p) QFontDatabase::font((f),(s),(p))
#define sysfont QFontDatabase::systemFont(QFontDatabase::GeneralFont)
#else
#define dbfont(f,s,p) qfontdatabase.font((f),(s),(p))
#define sysfont qfontdatabase.systemFont(QFontDatabase::GeneralFont)
#endif

class QXmojiWinPrivate {
    Q_DISABLE_COPY(QXmojiWinPrivate)
    Q_DECLARE_PUBLIC(QXmojiWin)
    QXmojiWin *const q_ptr;

    QXmojiWinPrivate(QXmojiWin *);
};

QXmojiWinPrivate::QXmojiWinPrivate(QXmojiWin *win) :
    q_ptr(win)
{
}

QXmojiWin::QXmojiWin() :
    QWidget(nullptr, Qt::WindowDoesNotAcceptFocus),
    d_ptr(new QXmojiWinPrivate(this))
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

    setAttribute(Qt::WA_AlwaysShowToolTips);
    QTabWidget *tabs = new QTabWidget(this);
    tabs->tabBar()->setFont(font);
    tabs->setStyleSheet("QTabBar::tab { padding: 4px; }");

    int i = 0;
    for (const EmojiGroup *group = emojigroups; group;
	    group = EmojiGroup_next(group))
    {
	QScrollArea *area = new QScrollArea(tabs);
	QWidget *emojis = new QWidget(area);
	FlowLayout *layout = new FlowLayout(emojis);
	for (const Emoji *emoji = EmojiGroup_emojis(group);
		emoji; emoji = Emoji_next(emoji))
	{
	    EmojiButton *button = new EmojiButton(emojis, emoji);
	    button->setFont(font);
	    layout->addWidget(button);
	}
	emojis->setLayout(layout);
	area->setWidget(emojis);
	area->setWidgetResizable(true);
	tabs->addTab(area, QString::fromUcs4(
		    Emoji_codepoints(EmojiGroup_emojis(group))));
	tabs->setTabToolTip(i++, EmojiGroup_name(group));
    }

    QHBoxLayout *box = new QHBoxLayout();
    box->setContentsMargins(0, 0, 0, 0);
    box->addWidget(tabs);
    setLayout(box);
}

QXmojiWin::~QXmojiWin() {}
