#ifndef QXMOJI_EMOJIBUTTON_H
#define QXMOJI_EMOJIBUTTON_H

#include "decl.h"

#include <QLabel>

C_CLASS_DECL(Emoji);
class QEvent;
class QEnterEvent;
class QMouseEvent;
class QShowEvent;

class EmojiButton: public QLabel
{
	Q_OBJECT

	const Emoji *_emoji;
	
    public:
	EmojiButton(QWidget *parent, const Emoji *emoji);
	const Emoji *emoji() const;
	void setEmoji(const Emoji *emoji);

    signals:
	void clicked();

    protected:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	void enterEvent(QEvent *ev);
#else
	void enterEvent(QEnterEvent *ev);
#endif
	void leaveEvent(QEvent *ev);
	void mousePressEvent(QMouseEvent *ev);
	void showEvent(QShowEvent *ev);
};

#endif
