#include "qxmojiwin.h"

#include "emoji.h"
#include "emojibutton.h"
#include "emojifont.h"
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

    QAction settings;
    QAction exit;
    SearchField search;
    FlowLayout *results;

    QXmojiWinPrivate(QXmojiWin *);
};

QXmojiWinPrivate::QXmojiWinPrivate(QXmojiWin *win) :
    q_ptr(win),
    settings("&Settings"),
    exit("E&xit")
{}

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
	EmojiButton *button = new EmojiButton(nullptr, nullptr);
	button->setFont(font->font());
	connect(button, &EmojiButton::clicked, [this, button](){
		emit clicked(button); });
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
		    emit clicked(button); });
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
    menu.addAction(&d->settings);
    menu.addAction(&d->exit);
    menu.exec(ev->globalPos());
}
