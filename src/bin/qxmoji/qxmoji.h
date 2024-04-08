#ifndef QXMOJI_QXMOJI_H
#define QXMOJI_QXMOJI_H

#include <QApplication>
#include <QScopedPointer>

class QXmojiPrivate;
class QXmoji: public QApplication
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(QXmoji)
	QScopedPointer<QXmojiPrivate> const d_ptr;

    public:
	QXmoji(int &argc, char **argv);
	~QXmoji();

	void show();
};

#endif
