#ifndef QXMOJI_QXMOJIWIN_H
#define QXMOJI_QXMOJIWIN_H

#include "decl.h"

#include <QScopedPointer>
#include <QWidget>

class EmojiButton;
class EmojiFont;
class QCloseEvent;
class QContextMenuEvent;
class QShowEvent;

class QXmojiWinPrivate;
class QXmojiWin: public QWidget
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(QXmojiWin)
	QScopedPointer<QXmojiWinPrivate> const d_ptr;

    public:
	QXmojiWin(const EmojiFont *font);
	~QXmojiWin();

    protected:
	void closeEvent(QCloseEvent *ev) override;
	void contextMenuEvent(QContextMenuEvent *ev) override;
	void showEvent(QShowEvent *ev) override;

    signals:
	void clicked(const EmojiButton *button);
	void grab();
	void ungrab();
	void showing();
	void closing();
	void settings();
	void exit();
};

#endif
