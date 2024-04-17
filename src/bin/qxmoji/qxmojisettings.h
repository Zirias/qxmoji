#ifndef QXMOJI_QXMOJISETTINGS_H
#define QXMOJI_QXMOJISETTINGS_H

#include <QObject>
#include <QScopedPointer>

#include "emojifont.h"
#include "qxmoji.h"

class QString;
class QSize;

class QXmojiSettingsPrivate;
class QXmojiSettings: public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(QXmojiSettings)
	QScopedPointer<QXmojiSettingsPrivate> const d_ptr;

    public:
	QXmojiSettings(const QString &file);
	~QXmojiSettings();

	const QString &history() const;
	EmojiFont::Scale scale() const;
	bool shown() const;
	bool singleInstance() const;
	const QSize &size() const;
	QXmoji::TrayMode trayMode() const;
	int wait() const;

    public slots:
	void setHistory(const QString &history);
	void setScale(EmojiFont::Scale scale);
	void setShown(bool shown);
	void setSingleInstance(bool singleInstance);
	void setSize(const QSize &size);
	void setTrayMode(QXmoji::TrayMode trayMode);
	void setWait(int wait);

    signals:
	void historyChanged(const QString &history);
	void historyChangedExt(const QString &history);
	void scaleChanged(EmojiFont::Scale scale);
	void scaleChangedExt(EmojiFont::Scale scale);
	void shownChanged(bool shown);
	void shownChangedExt(bool shown);
	void singleInstanceChanged(bool singleInstance);
	void singleInstanceChangedExt(bool singleInstance);
	void sizeChanged(const QSize &size);
	void sizeChangedExt(const QSize &size);
	void trayModeChanged(QXmoji::TrayMode trayMode);
	void trayModeChangedExt(QXmoji::TrayMode trayMode);
	void waitChanged(int wait);
	void waitChangedExt(int wait);
};

#endif
