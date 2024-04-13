#ifndef QXMOJI_QXMOJIWIN_H
#define QXMOJI_QXMOJIWIN_H

#include "decl.h"

#include <QScopedPointer>
#include <QWidget>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define qtnativeres long
#else
#define qtnativeres qintptr
#endif

class EmojiButton;
class EmojiFont;
class QByteArray;
class QCloseEvent;
class QContextMenuEvent;
class QMenu;
class QShowEvent;

class QXmojiWinPrivate;
class QXmojiWin: public QWidget
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(QXmojiWin)
	QScopedPointer<QXmojiWinPrivate> const d_ptr;

    public:
	QXmojiWin(QMenu *contextMenu, const EmojiFont *font);
	~QXmojiWin();

	void setCloseOnMinimize(bool close);
	void setHideInTaskbar(bool hide);

    protected:
	void closeEvent(QCloseEvent *ev) override;
	void contextMenuEvent(QContextMenuEvent *ev) override;
	void showEvent(QShowEvent *ev) override;
	bool nativeEvent(const QByteArray &eventType, void *message,
		qtnativeres *result) override;

    signals:
	void clicked(const EmojiButton *button);
	void closing(bool minimize);
	void closed(bool minimize);
};

#endif
