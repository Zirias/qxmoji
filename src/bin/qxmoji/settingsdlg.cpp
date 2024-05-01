#include "settingsdlg.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QMetaEnum>
#include <QSpinBox>
#include <QVBoxLayout>

SettingsDlg::SettingsDlg(QWidget *parent, QXmojiSettings *settings) :
    QDialog(parent)
{
    setWindowTitle(QDialog::tr("qXmoji settings"));

    QWidget *settingsForm = new QWidget(this);
    QFormLayout *layout = new QFormLayout(settingsForm);
    int l,t,r,b;
    layout->getContentsMargins(&l, &t, &r, &b);
    layout->setContentsMargins(0, t, 0, b);
    layout->setLabelAlignment(Qt::AlignRight);

    QLabel *singleLabel = new QLabel(QDialog::tr("Instance mode:"), this);
    singleLabel->setToolTip(QDialog::tr(
	    "In \"single\" mode, only one instance of qXmoji is allowed to\n"
	    "run per user on the local machine, and starting another one\n"
	    "will just bring the running instance to the front.\n"
	    "In \"multi\" mode, no checks for already running instances\n"
	    "are done."));
    QComboBox *singleSelect = new QComboBox(this);
    singleSelect->setToolTip(singleLabel->toolTip());
    singleSelect->addItem(QDialog::tr("Single"), true);
    singleSelect->addItem(QDialog::tr("Multi"), false);
    layout->addRow(singleLabel, singleSelect);

    QLabel *trayLabel = new QLabel(QDialog::tr("Tray mode:"), this);
    trayLabel->setToolTip(QDialog::tr(
	    "Disabled: Do not use a tray icon.\n"
	    "Enabled: Use a tray icon, qXmoji keeps running on window close.\n"
	    "Minimize: Use a tray icon with \"minimize to tray\", closing\n"
	    "the window still exits."));
    QComboBox *traySelect = new QComboBox(this);
    traySelect->setToolTip(trayLabel->toolTip());
    QMetaEnum trayEnum = QMetaEnum::fromType<QXmoji::TrayMode>();
    for (int i = 0; i < trayEnum.keyCount(); ++i)
    {
	traySelect->addItem(QCoreApplication::translate("TrayMode",
		    trayEnum.key(i)), trayEnum.value(i));
    }
    layout->addRow(trayLabel, traySelect);

    QLabel *scaleLabel = new QLabel(QDialog::tr("Scale size:"), this);
    scaleLabel->setToolTip(QDialog::tr(
	    "Scale up the size of displayed Emojis.\n"
	    "Tiny means no scaling (same size as default window font)"));
    QComboBox *scaleSelect = new QComboBox(this);
    scaleSelect->setToolTip(scaleLabel->toolTip());
    QMetaEnum scaleEnum = QMetaEnum::fromType<EmojiFont::Scale>();
    for (int i = 0; i < scaleEnum.keyCount(); ++i)
    {
	scaleSelect->addItem(QCoreApplication::translate("Scale",
		    scaleEnum.key(i)), scaleEnum.value(i));
    }
    layout->addRow(scaleLabel, scaleSelect);

    QLabel *waitLabel = new QLabel(
	    QDialog::tr("Keymap reset wait (ms):"), this);
    waitLabel->setToolTip(QDialog::tr(
	    "qXmoji sends emoji codepoints as key press events.\n"
	    "Therefore, the keyboard mapping must be temporarily changed.\n"
	    "This is the time in ms to wait before restoring the mapping\n"
	    "after sending the keyboard events.\n"
	    "Raising this might help if emojis don't \"arrive\" at all or you\n"
	    "see basic (ASCII) characters instead.\n"
	    "It won't help when a single emoji is shown as two or more\n"
	    "symbols, this is handled by the receiving client."));
    QSpinBox *waitSelect = new QSpinBox(this);
    waitSelect->setToolTip(waitLabel->toolTip());
    waitSelect->setMinimum(0);
    waitSelect->setMaximum(500);
    waitSelect->setSingleStep(10);
    layout->addRow(waitLabel, waitSelect);

    settingsForm->setLayout(layout);

    QVBoxLayout *dlgLayout = new QVBoxLayout(this);
    dlgLayout->addWidget(settingsForm);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttons, &QDialogButtonBox::accepted, this, &QWidget::hide);
    dlgLayout->addWidget(buttons);
    setLayout(dlgLayout);

    connect(singleSelect, QOverload<int>::of(&QComboBox::activated),
	    [settings, singleSelect](){
		settings->setSingleInstance(
			singleSelect->currentData().toBool());
	    });
    connect(traySelect, QOverload<int>::of(&QComboBox::activated),
	    [settings, traySelect](){
		settings->setTrayMode(
			traySelect->currentData().value<QXmoji::TrayMode>());
	    });
    connect(scaleSelect, QOverload<int>::of(&QComboBox::activated),
	    [settings, scaleSelect](){
		settings->setScale(
			scaleSelect->currentData().value<EmojiFont::Scale>());
	    });
    connect(waitSelect, QOverload<int>::of(&QSpinBox::valueChanged),
	    [settings](int ms){ settings->setWait(ms); });


    auto setSingleInstance = [singleSelect](bool singleInstance){
	int index = singleSelect->findData(singleInstance);
	if (index >= 0) singleSelect->setCurrentIndex(index);
    };
    setSingleInstance(settings->singleInstance());
    connect(settings, &QXmojiSettings::singleInstanceChangedExt,
	    setSingleInstance);

    auto setTrayMode = [traySelect](QXmoji::TrayMode mode){
	int index = traySelect->findData(mode);
	if (index >= 0) traySelect->setCurrentIndex(index);
    };
    setTrayMode(settings->trayMode());
    connect(settings, &QXmojiSettings::trayModeChangedExt, setTrayMode);

    auto setScale = [scaleSelect](EmojiFont::Scale scale){
	int index = scaleSelect->findData(scale);
	if (index >= 0) scaleSelect->setCurrentIndex(index);
    };
    setScale(settings->scale());
    connect(settings, &QXmojiSettings::scaleChangedExt, setScale);

    auto setWait = [waitSelect](int ms){ waitSelect->setValue(ms); };
    setWait(settings->wait());
    connect(settings, &QXmojiSettings::waitChangedExt, setWait);
}

