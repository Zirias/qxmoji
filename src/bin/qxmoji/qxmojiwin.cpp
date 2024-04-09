#include "qxmojiwin.h"

#include "emoji.h"
#include "emojibutton.h"
#include "emojifont.h"
#include "flowlayout.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QMenu>
#include <QScrollArea>
#include <QTabBar>
#include <QTabWidget>

class QXmojiWinPrivate {
    Q_DISABLE_COPY(QXmojiWinPrivate)
    Q_DECLARE_PUBLIC(QXmojiWin)
    QXmojiWin *const q_ptr;

    QAction settings;
    QAction exit;

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

    QHBoxLayout *box = new QHBoxLayout();
    box->setContentsMargins(0, 0, 0, 0);
    box->addWidget(tabs);
    setLayout(box);

    connect(&d_ptr->settings, &QAction::triggered, [this](){
	    emit settings(); });
    connect(&d_ptr->exit, &QAction::triggered, [this](){
	    emit exit(); });
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
