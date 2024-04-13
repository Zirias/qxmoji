#ifndef QXMOJI_SETTINGSDLG_H
#define QXMOJI_SETTINGSDLG_H

#include "emojifont.h"
#include "qxmoji.h"

#include <QDialog>
#include <QScopedPointer>

class SettingsDlgPrivate;
class SettingsDlg: public QDialog
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(SettingsDlg)
	QScopedPointer<SettingsDlgPrivate> const d_ptr;

    public:
	SettingsDlg(QWidget *parent);
	~SettingsDlg();

    public slots:
	void setSingleInstance(bool singleInstance);
	void setTrayMode(QXmoji::TrayMode mode);
	void setScale(EmojiFont::Scale scale);
	void setWaitMs(int waitMs);

    signals:
	void singleInstanceChanged(bool singleInstance);
	void trayModeChanged(QXmoji::TrayMode mode);
	void scaleChanged(EmojiFont::Scale scale);
	void waitMsChanged(int waitMs);
};

#endif
