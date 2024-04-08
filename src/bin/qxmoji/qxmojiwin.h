#ifndef QXMOJI_QXMOJIWIN_H
#define QXMOJI_QXMOJIWIN_H

#include <QScopedPointer>
#include <QWidget>

class QXmojiWinPrivate;
class QXmojiWin: public QWidget
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(QXmojiWin)
	QScopedPointer<QXmojiWinPrivate> const d_ptr;

    public:
	QXmojiWin();
	~QXmojiWin();
};

#endif
