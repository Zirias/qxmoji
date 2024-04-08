#ifndef QXMOJI_EMOJIBUTTON_H
#define QXMOJI_EMOJIBUTTON_H

#include "decl.h"

#include <QLabel>

C_CLASS_DECL(Emoji);
class QEvent;
class QMouseEvent;
class QShowEvent;

class EmojiButton: public QLabel
{
	Q_OBJECT

	const Emoji *_emoji;
	
    public:
	EmojiButton(QWidget *parent, const Emoji *emoji);
	const Emoji *emoji() const;

    signals:
	void clicked();

    protected:
	void enterEvent(QEvent *ev);
	void leaveEvent(QEvent *ev);
	void mousePressEvent(QMouseEvent *ev);
	void showEvent(QShowEvent *ev);
};

#endif
