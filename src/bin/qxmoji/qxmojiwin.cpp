#include "qxmojiwin.h"

#include "emoji.h"
#include "emojibutton.h"
#include "emojifont.h"
#include "emojihistory.h"
#include "emojisearch.h"
#include "flowlayout.h"
#include "searchfield.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QMenu>
#include <QScrollArea>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>

class QXmojiWinPrivate {
    Q_DISABLE_COPY(QXmojiWinPrivate)
    Q_DECLARE_PUBLIC(QXmojiWin)
    QXmojiWin *const q_ptr;

    QAction about;
    QAction settings;
    QAction exit;
    SearchField search;
    FlowLayout *results;
    FlowLayout *history;

    QXmojiWinPrivate(QXmojiWin *);
    void emojiClicked(const EmojiButton *button);
};

QXmojiWinPrivate::QXmojiWinPrivate(QXmojiWin *win) :
    q_ptr(win),
    about("&About"),
    settings("&Settings"),
    exit("E&xit")
{}

void QXmojiWinPrivate::emojiClicked(const EmojiButton *button)
{
    Q_Q(QXmojiWin);
    emit q->clicked(button);
    Emoji_use(button->emoji());
    const Emoji **h = Emoji_history();
    for (int i = 0; i < EMOJIHISTORY_MAXLEN; ++i)
    {
	EmojiButton *hb = static_cast<EmojiButton *>(
		history->itemAt(i)->widget());
	hb->setEmoji(h[i]);
    }
}

QXmojiWin::QXmojiWin(const EmojiFont *font) :
    QWidget(nullptr, Qt::WindowDoesNotAcceptFocus),
    d_ptr(new QXmojiWinPrivate(this))
{
    setAttribute(Qt::WA_AlwaysShowToolTips);
    QTabWidget *tabs = new QTabWidget(this);
    tabs->tabBar()->setFont(font->font());
    tabs->setStyleSheet("QTabBar::tab { padding: 4px; }");
    connect(font, &EmojiFont::fontChanged, [font, tabs](){
	    tabs->tabBar()->setFont(font->font()); });

    int i = 0;
    QWidget *searchTab = new QWidget(tabs);
    QVBoxLayout *searchLayout = new QVBoxLayout(searchTab);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->addWidget(&d_ptr->search);
    QScrollArea *searchArea = new QScrollArea(searchTab);
    QWidget *searchResults = new QWidget(searchArea);
    d_ptr->results = new FlowLayout(searchResults);
    for (int i = 0; i < EMOJISEARCH_MAXRESULTS; ++i)
    {
	EmojiButton *button = new EmojiButton(searchResults, nullptr);
	button->setFont(font->font());
	connect(button, &EmojiButton::clicked, [this, button](){
		d_ptr->emojiClicked(button); });
	connect(font, &EmojiFont::fontChanged, [font, button](){
		button->setFont(font->font()); });
	d_ptr->results->addWidget(button);
    }
    searchResults->setLayout(d_ptr->results);
    searchArea->setWidget(searchResults);
    searchArea->setWidgetResizable(true);
    searchLayout->addWidget(searchArea);
    searchTab->setLayout(searchLayout);
    tabs->addTab(searchTab, QString::fromUcs4(U"\x1f50d"));
    tabs->setTabToolTip(i++, "Search");

    QScrollArea *historyArea = new QScrollArea(tabs);
    QWidget *historyPane = new QWidget(historyArea);
    d_ptr->history = new FlowLayout(historyPane);
    historyPane->setLayout(d_ptr->history);
    historyArea->setWidget(historyPane);
    historyArea->setWidgetResizable(true);
    tabs->addTab(historyArea, QString::fromUtf16(u"\x23f3"));
    tabs->setTabToolTip(i++, "History");

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
	    button->setFont(font->font());
	    connect(button, &EmojiButton::clicked, [this, button](){
		    d_ptr->emojiClicked(button); });
	    connect(font, &EmojiFont::fontChanged, [font, button](){
		    button->setFont(font->font()); });
	    layout->addWidget(button);
	}
	emojis->setLayout(layout);
	area->setWidget(emojis);
	area->setWidgetResizable(true);
	tabs->addTab(area, QString::fromUcs4(
		    Emoji_codepoints(EmojiGroup_emojis(group))));
	tabs->setTabToolTip(i++, EmojiGroup_name(group));
    }
    tabs->setCurrentIndex(1);

    QHBoxLayout *box = new QHBoxLayout();
    box->setContentsMargins(0, 0, 0, 0);
    box->addWidget(tabs);
    setLayout(box);

    connect(&d_ptr->about, &QAction::triggered, [this](){
	    emit about(); });
    connect(&d_ptr->settings, &QAction::triggered, [this](){
	    emit settings(); });
    connect(&d_ptr->exit, &QAction::triggered, [this](){
	    emit exit(); });
    connect(&d_ptr->search, &SearchField::activated, [this](){
	    emit grab(); });
    connect(&d_ptr->search, &SearchField::left, [this](){
	    emit ungrab(); });
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
    connect(this, &QXmojiWin::showing, [this, historyPane, font]() {
		const Emoji **history = Emoji_history();
		for (int i = 0; i < EMOJIHISTORY_MAXLEN; ++i)
		{
		    EmojiButton *button = new EmojiButton(historyPane,
			    history[i]);
		    button->setFont(font->font());
		    connect(button, &EmojiButton::clicked, [this, button](){
			    d_ptr->emojiClicked(button); });
		    connect(font, &EmojiFont::fontChanged, [font, button](){
			    button->setFont(font->font()); });
		    d_ptr->history->addWidget(button);
		}
	    });
}

QXmojiWin::~QXmojiWin() {}

void QXmojiWin::closeEvent(QCloseEvent *ev)
{
    emit closing();
    QWidget::closeEvent(ev);
}

void QXmojiWin::contextMenuEvent(QContextMenuEvent *ev)
{
    Q_D(QXmojiWin);
    QMenu menu(this);
    menu.addAction(&d->about);
    menu.addAction(&d->settings);
    menu.addAction(&d->exit);
    menu.exec(ev->globalPos());
}

void QXmojiWin::showEvent(QShowEvent *ev)
{
    Q_D(QXmojiWin);
    if (d->history->count() == 0) emit showing();
    QWidget::showEvent(ev);
}

