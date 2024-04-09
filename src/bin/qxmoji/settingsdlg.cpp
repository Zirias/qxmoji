#include "settingsdlg.h"

#include <QFormLayout>
#include <QLabel>
#include <QMetaEnum>
#include <QComboBox>

class SettingsDlgPrivate
{
    Q_DISABLE_COPY(SettingsDlgPrivate)
    Q_DECLARE_PUBLIC(SettingsDlg)
    SettingsDlg *const q_ptr;

    QLabel scaleLabel;
    QComboBox scaleSelect;

    SettingsDlgPrivate(SettingsDlg *);
};

SettingsDlgPrivate::SettingsDlgPrivate(SettingsDlg *dlg) :
    q_ptr(dlg),
    scaleLabel("Scale size:")
{
    QMetaEnum scaleEnum = QMetaEnum::fromType<EmojiFont::Scale>();
    for (int i = 0; i < scaleEnum.keyCount(); ++i)
    {
	scaleSelect.addItem(scaleEnum.key(i), scaleEnum.value(i));
    }
}

SettingsDlg::SettingsDlg(QWidget *parent) :
    QDialog(parent),
    d_ptr(new SettingsDlgPrivate(this))
{
    setWindowTitle("qXmoji settings");
    QFormLayout *layout = new QFormLayout(this);
    layout->addRow(&d_ptr->scaleLabel, &d_ptr->scaleSelect);
    setLayout(layout);

    connect(&d_ptr->scaleSelect, QOverload<int>::of(&QComboBox::activated),
	    [this](){ emit scaleChanged(
		d_ptr->scaleSelect.currentData().value<EmojiFont::Scale>());
	    });
}

SettingsDlg::~SettingsDlg() {}

void SettingsDlg::setScale(EmojiFont::Scale scale)
{
    Q_D(SettingsDlg);
    int index = d->scaleSelect.findData(scale);
    if (index >= 0) d->scaleSelect.setCurrentIndex(index);
}

