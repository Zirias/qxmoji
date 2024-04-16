#ifndef QXMOJI_EMOJIHISTORY_H
#define QXMOJI_EMOJIHISTORY_H

#include "decl.h"

#include <QObject>
#include <QScopedPointer>

#define EMOJIHISTORY_MAXLEN 100

C_CLASS_DECL(Emoji);

class EmojiHistoryPrivate;
class EmojiHistory: public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(EmojiHistory)
	QScopedPointer<EmojiHistoryPrivate> const d_ptr;

    public:
	EmojiHistory(const QString &str);
	~EmojiHistory();

	int len();
	const Emoji *at(int i);
	const QString &serialize();

    public slots:
	void record(const Emoji *emoji);

    signals:
	void changed(int changeLen);
};

#endif
