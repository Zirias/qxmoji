#include "settingsdlg.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QMetaEnum>
#include <QSpinBox>
#include <QVBoxLayout>

class SettingsDlgPrivate
{
    Q_DISABLE_COPY(SettingsDlgPrivate)
    Q_DECLARE_PUBLIC(SettingsDlg)
    SettingsDlg *const q_ptr;

    QLabel singleLabel;
    QComboBox singleSelect;
    QLabel scaleLabel;
    QComboBox scaleSelect;
    QLabel waitLabel;
    QSpinBox waitSelect;

    SettingsDlgPrivate(SettingsDlg *);
};

SettingsDlgPrivate::SettingsDlgPrivate(SettingsDlg *dlg) :
    q_ptr(dlg),
    singleLabel("Instance mode:"),
    scaleLabel("Scale size:"),
    waitLabel("Keymap reset wait (ms):")
{
    singleLabel.setToolTip(
	    "In \"single\" mode, only one instance of qXmoji is allowed to\n"
	    "run per user on the local machine, and starting another one\n"
	    "will just bring the running instance to the front.\n"
	    "In \"multi\" mode, no checks for already running instances\n"
	    "are done.");
    singleSelect.setToolTip(singleLabel.toolTip());
    singleSelect.addItem("Single", true);
    singleSelect.addItem("Multi", false);
    scaleLabel.setToolTip("Scale up the size of displayed Emojis.\n"
	    "Tiny means no scaling (same size as default window font)");
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
    QWidget *settingsForm = new QWidget(this);
    QFormLayout *layout = new QFormLayout(settingsForm);
    int l,t,r,b;
    layout->getContentsMargins(&l, &t, &r, &b);
    layout->setContentsMargins(0, t, 0, b);
    layout->setLabelAlignment(Qt::AlignRight);
    layout->addRow(&d_ptr->singleLabel, &d_ptr->singleSelect);
    layout->addRow(&d_ptr->scaleLabel, &d_ptr->scaleSelect);
    layout->addRow(&d_ptr->waitLabel, &d_ptr->waitSelect);
    settingsForm->setLayout(layout);

    QVBoxLayout *dlgLayout = new QVBoxLayout(this);
    dlgLayout->addWidget(settingsForm);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttons, &QDialogButtonBox::accepted, this, &QWidget::hide);
    dlgLayout->addWidget(buttons);
    setLayout(dlgLayout);

    connect(&d_ptr->singleSelect, QOverload<int>::of(&QComboBox::activated),
	    [this](){ emit singleInstanceChanged(
		d_ptr->singleSelect.currentData().toBool());
	    });
    connect(&d_ptr->scaleSelect, QOverload<int>::of(&QComboBox::activated),
	    [this](){ emit scaleChanged(
		d_ptr->scaleSelect.currentData().value<EmojiFont::Scale>());
	    });
    connect(&d_ptr->waitSelect, QOverload<int>::of(&QSpinBox::valueChanged),
	    [this](int ms){ emit waitMsChanged(ms); });
}

SettingsDlg::~SettingsDlg() {}

void SettingsDlg::setSingleInstance(bool singleInstance)
{
    Q_D(SettingsDlg);
    int index = d->singleSelect.findData(singleInstance);
    if (index >= 0) d->singleSelect.setCurrentIndex(index);
}

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

