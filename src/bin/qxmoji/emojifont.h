#ifndef QXMOJI_EMOJIFONT_H
#define QXMOJI_EMOJIFONT_H

#include <QObject>
#include <QScopedPointer>

class QFont;

class EmojiFontPrivate;
class EmojiFont: public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(EmojiFont)
	QScopedPointer<EmojiFontPrivate> const d_ptr;

    public:
	EmojiFont();
	~EmojiFont();

	enum Scale { Tiny, Small, Medium, Large, Huge };
	Q_ENUM(Scale)

	const QFont &font() const;
	void setScale(Scale scale);

    signals:
	void fontChanged();
};

#endif
