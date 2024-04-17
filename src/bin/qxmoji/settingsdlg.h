#ifndef QXMOJI_SETTINGSDLG_H
#define QXMOJI_SETTINGSDLG_H

#include "qxmojisettings.h"

#include <QDialog>

class SettingsDlg: public QDialog
{
    public:
	SettingsDlg(QWidget *parent, QXmojiSettings *settings);
};

#endif
