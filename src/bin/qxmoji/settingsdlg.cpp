#include "settingsdlg.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QMetaEnum>
#include <QSpinBox>

class SettingsDlgPrivate
{
    Q_DISABLE_COPY(SettingsDlgPrivate)
    Q_DECLARE_PUBLIC(SettingsDlg)
    SettingsDlg *const q_ptr;

    QLabel scaleLabel;
    QComboBox scaleSelect;
    QLabel waitLabel;
    QSpinBox waitSelect;

    SettingsDlgPrivate(SettingsDlg *);
};

SettingsDlgPrivate::SettingsDlgPrivate(SettingsDlg *dlg) :
    q_ptr(dlg),
    scaleLabel("Scale size:"),
    waitLabel("Keymap reset wait (ms):")
{
    scaleLabel.setToolTip("Scale up the size of displayed Emojis.\n"
	    "Tiny means no scaling (same size as default Window font)");
    scaleSelect.setToolTip(scaleLabel.toolTip());
    QMetaEnum scaleEnum = QMetaEnum::fromType<EmojiFont::Scale>();
    for (int i = 0; i < scaleEnum.keyCount(); ++i)
    {
	scaleSelect.addItem(scaleEnum.key(i), scaleEnum.value(i));
    }
    waitLabel.setToolTip("qXmoji sends emoji codepoints as key press events.\n"
	    "Therefore, the keyboard mapping must be temporarily changed.\n"
	    "This is the time in ms to wait before restoring the mapping\n"
	    "after sending the keyboard events.\n"
	    "\n"
	    "Raising this might help if emojis don't \"arrive\" at all or you\n"
	    "see basic (ASCII) characters instead.\n"
	    "It won't help when a single emoji is shown as two or more\n"
	    "symbols, this is handled by the receiving client.");
    waitSelect.setToolTip(waitLabel.toolTip());
    waitSelect.setMinimum(0);
    waitSelect.setMaximum(500);
    waitSelect.setSingleStep(10);
}

SettingsDlg::SettingsDlg(QWidget *parent) :
    QDialog(parent),
    d_ptr(new SettingsDlgPrivate(this))
{
    setWindowTitle("qXmoji settings");
    QFormLayout *layout = new QFormLayout(this);
    layout->setLabelAlignment(Qt::AlignRight);
    layout->addRow(&d_ptr->scaleLabel, &d_ptr->scaleSelect);
    layout->addRow(&d_ptr->waitLabel, &d_ptr->waitSelect);
    setLayout(layout);

    connect(&d_ptr->scaleSelect, QOverload<int>::of(&QComboBox::activated),
	    [this](){ emit scaleChanged(
		d_ptr->scaleSelect.currentData().value<EmojiFont::Scale>());
	    });
    connect(&d_ptr->waitSelect, QOverload<int>::of(&QSpinBox::valueChanged),
	    [this](int ms){ emit waitMsChanged(ms); });
}

SettingsDlg::~SettingsDlg() {}

void SettingsDlg::setScale(EmojiFont::Scale scale)
{
    Q_D(SettingsDlg);
    int index = d->scaleSelect.findData(scale);
    if (index >= 0) d->scaleSelect.setCurrentIndex(index);
}

void SettingsDlg::setWaitMs(int waitMs)
{
    Q_D(SettingsDlg);
    d->waitSelect.setValue(waitMs);
}

