#ifndef QXMOJI_QXMOJI_H
#define QXMOJI_QXMOJI_H

#include "decl.h"

#include <QApplication>
#include <QScopedPointer>

C_CLASS_DECL(XcbAdapter);

class QXmojiPrivate;
class QXmoji: public QApplication
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(QXmoji)
	QScopedPointer<QXmojiPrivate> const d_ptr;

    public:
	QXmoji(int &argc, char **argv, void (*started)());
	~QXmoji();

	enum TrayMode { Disabled, Enabled, Minimize };
	Q_ENUM(TrayMode)

	XcbAdapter *xcb();
};

#define qXm (reinterpret_cast<QXmoji *>(qApp))

#endif
