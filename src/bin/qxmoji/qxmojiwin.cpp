#include "qxmojiwin.h"

#include "emoji.h"
#include "emojibutton.h"
#include "emojifont.h"
#include "emojihistory.h"
#include "emojisearch.h"
#include "emojistring.h"
#include "flowlayout.h"
#include "qxmoji.h"
#include "searchfield.h"
#include "xcbadapter.h"

#include <QCloseEvent>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QList>
#include <QMenu>
#include <QQueue>
#include <QScrollArea>
#include <QShowEvent>
#include <QTabBar>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWindow>

class InitEmojiQueue
{
	struct Params
	{
	    EmojiButton *button;
	    const Emoji *emoji;
	};

	QQueue<Params> _queue;

    public:
	InitEmojiQueue() {};

	void enqueue(EmojiButton *button, const Emoji *emoji)
	{
	    _queue.enqueue({button, emoji});
	}

	template <typename Functor> void run(Functor done)
	{
	    if (_queue.count())
	    {
		QTimer::singleShot(1, [this, done](){
			struct Params p = _queue.dequeue();
			p.button->setEmoji(p.emoji);
			run(done);
		    });
	    }
	    else done();
	}
};

class QXmojiWinPrivate {
    Q_DISABLE_COPY(QXmojiWinPrivate)
    Q_DECLARE_PUBLIC(QXmojiWin)
    QXmojiWin *const q_ptr;

    QPoint pos;
    SearchField search;
    const EmojiFont *font;
    FlowLayout *results;
    QMenu *contextMenu;
    bool closeOnMinimize;
    bool closeIsMinimize;
    bool hideInTaskbar;

    QXmojiWinPrivate(QXmojiWin *, const EmojiFont *, QMenu *);
};

QXmojiWinPrivate::QXmojiWinPrivate(QXmojiWin *win, const EmojiFont *font,
	QMenu *contextMenu) :
    q_ptr(win),
    pos(-1, -1),
    font(font),
    contextMenu(contextMenu),
    closeOnMinimize(false),
    closeIsMinimize(false),
    hideInTaskbar(false)
{}

QXmojiWin::QXmojiWin(QMenu *contextMenu, const EmojiFont *font,
	EmojiHistory *history) :
    QWidget(nullptr, Qt::WindowDoesNotAcceptFocus),
    d_ptr(new QXmojiWinPrivate(this, font, contextMenu))
{
    setAttribute(Qt::WA_AlwaysShowToolTips);
    QTabWidget *tabs = new QTabWidget(this);
    tabs->tabBar()->setFont(font->font());
    tabs->setStyleSheet("QTabBar::tab { padding: 4px; }");
    connect(font, &EmojiFont::fontChanged, [font, tabs](){
	    tabs->tabBar()->setFont(font->font()); });

    qsizetype nbuttons = EMOJISEARCH_MAXRESULTS + EMOJIHISTORY_MAXLEN
	+ Emoji_count();
    QList<EmojiButton *> buttons(nbuttons);
    for (qsizetype bn = 0; bn < nbuttons; ++bn)
    {
	EmojiButton *btn = new EmojiButton(this);
	btn->setFont(font->font());
	connect(btn, &EmojiButton::clicked, [this, btn](){
		const Emoji *emoji = btn->emoji();
		if (emoji) emit emojiSelected(emoji); });
	connect(font, &EmojiFont::fontChanged, [font, btn](){
		btn->setFont(font->font()); });
	buttons[bn] = btn;
    }

    int tn = 0;
    qsizetype bn = 0;
    QWidget *searchTab = new QWidget(tabs);
    QVBoxLayout *searchLayout = new QVBoxLayout(searchTab);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->addWidget(&d_ptr->search);
    QScrollArea *searchArea = new QScrollArea(searchTab);
    QWidget *searchResults = new QWidget(searchArea);
    d_ptr->results = new FlowLayout(searchResults);
    for (int i = 0; i < EMOJISEARCH_MAXRESULTS; ++i)
    {
	d_ptr->results->addWidget(buttons[bn++]);
    }
    searchResults->setLayout(d_ptr->results);
    searchArea->setWidget(searchResults);
    searchArea->setWidgetResizable(true);
    searchLayout->addWidget(searchArea);
    searchTab->setLayout(searchLayout);
    tabs->addTab(searchTab, QString::fromUcs4(U"\x1f50d"));
    tabs->setTabToolTip(tn++, "Search");

    QScrollArea *historyArea = new QScrollArea(tabs);
    QWidget *historyPane = new QWidget(historyArea);
    FlowLayout *historylayout = new FlowLayout(historyPane);
    for (int i = 0; i < EMOJIHISTORY_MAXLEN; ++i)
    {
	EmojiButton *btn = buttons[bn++];
	btn->setEmoji(history->at(i));
	historylayout->addWidget(btn);
    }
    historyPane->setLayout(historylayout);
    historyArea->setWidget(historyPane);
    historyArea->setWidgetResizable(true);
    tabs->addTab(historyArea, QString::fromUtf16(u"\x23f3"));
    tabs->setTabToolTip(tn++, "History");

    InitEmojiQueue *ieq = new InitEmojiQueue();
    for (size_t n = 0; n < EmojiGroup_count(); ++n)
    {
	const EmojiGroup *group = EmojiGroup_at(n);
	QScrollArea *area = new QScrollArea(tabs);
	QWidget *emojis = new QWidget(area);
	FlowLayout *layout = new FlowLayout(emojis);
	for (size_t m = 0; m < EmojiGroup_countEmojis(group); ++m)
	{
	    const Emoji *emoji = EmojiGroup_emoji(group, m);
	    EmojiButton *btn = buttons[bn++];
	    ieq->enqueue(btn, emoji);
	    layout->addWidget(btn);
	}
	emojis->setLayout(layout);
	area->setWidget(emojis);
	area->setWidgetResizable(true);
	tabs->addTab(area, Emoji_qstr(EmojiGroup_emoji(group, 0)));
	tabs->setTabToolTip(tn++, EmojiGroup_name(group));
    }
    tabs->setCurrentIndex(1);

    QHBoxLayout *box = new QHBoxLayout();
    box->setContentsMargins(0, 0, 0, 0);
    box->addWidget(tabs);
    setLayout(box);

    connect(&d_ptr->search, &SearchField::activated, [this](){
	    XcbAdapter_grabKeyboard(qXm->xcb(), winId()); });
    connect(&d_ptr->search, &SearchField::left, [](){
	    XcbAdapter_ungrabKeyboard(qXm->xcb()); });
    connect(&d_ptr->search, &QLineEdit::textEdited,
	    [this](const QString &text){
		int i = 0;
		if (text.length() >= 2)
		{
		    const char *pat = text.toLocal8Bit().constData();
		    const Emoji **res = Emoji_search(pat);
		    while (*res && i < EMOJISEARCH_MAXRESULTS)
		    {
			EmojiButton *b = static_cast<EmojiButton *>(
				d_ptr->results->itemAt(i)->widget());
			b->setEmoji(*res);
			++i;
			++res;
		    }
		}
		while (i < EMOJISEARCH_MAXRESULTS)
		{
		    EmojiButton *b = static_cast<EmojiButton *>(
			    d_ptr->results->itemAt(i)->widget());
		    if (!b->emoji()) break;
		    b->setEmoji(nullptr);
		    ++i;
		}
	    });
    connect(history, &EmojiHistory::changed,
	    [history, historylayout](int changeLen){
		for (int i = 0; i < changeLen; ++i)
		{
		    EmojiButton *hb = static_cast<EmojiButton *>(
			    historylayout->itemAt(i)->widget());
		    hb->setEmoji(history->at(i));
		}
	    });

    ieq->run([ieq](){ delete ieq; });
}

QXmojiWin::~QXmojiWin() {}

void QXmojiWin::setCloseOnMinimize(bool close)
{
    Q_D(QXmojiWin);
    d->closeOnMinimize = close;
}

void QXmojiWin::setHideInTaskbar(bool hide)
{
    Q_D(QXmojiWin);
    if (hide == d->hideInTaskbar) return;
    d->hideInTaskbar = hide;
    setAttribute(Qt::WA_X11NetWmWindowTypeUtility, hide);
    XcbAdapter_setSkipTaskbar(qXm->xcb(), winId(), hide);
}

void QXmojiWin::closeEvent(QCloseEvent *ev)
{
    Q_D(QXmojiWin);
    emit closing(d->closeIsMinimize);
    d->pos = windowHandle()->position();
    QWidget::closeEvent(ev);
    emit closed(d->closeIsMinimize);
}

void QXmojiWin::contextMenuEvent(QContextMenuEvent *ev)
{
    Q_D(QXmojiWin);
    d->contextMenu->exec(ev->globalPos());
}

void QXmojiWin::showEvent(QShowEvent *ev)
{
    Q_D(QXmojiWin);
    setAttribute(Qt::WA_X11NetWmWindowTypeUtility, d->hideInTaskbar);
    XcbAdapter_setSkipTaskbar(qXm->xcb(), winId(), d->hideInTaskbar);
    if (d->pos != QPoint(-1, -1)) windowHandle()->setPosition(d->pos);
    QWidget::showEvent(ev);
    d->closeIsMinimize = false;
}

bool QXmojiWin::nativeEvent(const QByteArray &eventType,
	void *message, qtnativeres *result)
{
    Q_D(QXmojiWin);
    (void)eventType;
    (void)result;

    if (!d->closeOnMinimize) return false;
    xcb_generic_event_t *ev = reinterpret_cast<xcb_generic_event_t *>(message);
    bool blocked = XcbAdapter_blockMinimizeEvent(qXm->xcb(), winId(), ev);
    if (blocked)
    {
	d->closeIsMinimize = true;
	QMetaObject::invokeMethod(this, &QWidget::close, Qt::QueuedConnection);
    }
    return blocked;
}

