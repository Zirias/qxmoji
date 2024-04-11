#include "qxmoji.h"

#include "aboutdlg.h"
#include "emojibutton.h"
#include "emojifont.h"
#include "emojihistory.h"
#include "qxmojiwin.h"
#include "settingsdlg.h"
#include "xkeyinjector.h"

#include <QDir>
#include <QIcon>
#include <QSettings>

class QXmojiPrivate {
    Q_DISABLE_COPY(QXmojiPrivate)
    Q_DECLARE_PUBLIC(QXmoji)
    QXmoji *const q_ptr;

    EmojiFont font;
    QIcon appIcon;
    QSettings settings;
    QXmojiWin win;
    AboutDlg aboutDlg;
    SettingsDlg settingsDlg;
    
    QXmojiPrivate(QXmoji *);
};

QXmojiPrivate::QXmojiPrivate(QXmoji *app) :
    q_ptr(app),
    settings(QDir::homePath() + "/.config/qxmoji.ini", QSettings::IniFormat),
    win(&font),
    aboutDlg(&win),
    settingsDlg(&win)
{
    appIcon.addPixmap(QPixmap(":/icon_48.png"));
    appIcon.addPixmap(QPixmap(":/icon_32.png"));
    appIcon.addPixmap(QPixmap(":/icon_16.png"));
    win.setWindowIcon(appIcon);
    aboutDlg.setWindowIcon(appIcon);
    settingsDlg.setWindowIcon(appIcon);
    EmojiFont::Scale scale = settings.value(
	    "scale", EmojiFont::Scale::Small).value<EmojiFont::Scale>();
    font.setScale(scale);
    settingsDlg.setScale(scale);
    int waitMs = settings.value("wait", 50).toInt();
    if (waitMs < 0) waitMs = 0;
    if (waitMs > 500) waitMs = 500;
    XKeyInjector_setWaitMs(waitMs);
    settingsDlg.setWaitMs(waitMs);
    QByteArray history = settings.value("history", QByteArray()).toByteArray();
    Emoji_loadHistory(history.length(), history);
}

QXmoji::QXmoji(int &argc, char **argv) :
    QApplication(argc, argv),
    d_ptr(new QXmojiPrivate(this))
{
    connect(&d_ptr->win, &QXmojiWin::clicked, [](const EmojiButton *b){
	    XKeyInjector_inject(b->emoji()); });
    connect(&d_ptr->win, &QXmojiWin::grab, [](){
	    XKeyInjector_grabKeyboard(); });
    connect(&d_ptr->win, &QXmojiWin::ungrab, [](){
	    XKeyInjector_ungrabKeyboard(); });
    connect(&d_ptr->win, &QXmojiWin::about,
	    &d_ptr->aboutDlg, &QWidget::show);
    connect(&d_ptr->win, &QXmojiWin::settings,
	    &d_ptr->settingsDlg, &QWidget::show);
    connect(&d_ptr->win, &QXmojiWin::exit, QApplication::quit);
    connect(&d_ptr->win, &QXmojiWin::closing,
	    [this](){
		d_ptr->settings.setValue("size", d_ptr->win.size());
		size_t historysz;
		const void *historybytes = Emoji_saveHistory(&historysz);
		QByteArray history((const char *)historybytes, historysz);
		d_ptr->settings.setValue("history", history);
	    });
    connect(&d_ptr->settingsDlg, &SettingsDlg::scaleChanged,
	    &d_ptr->font, &EmojiFont::setScale);
    connect(&d_ptr->settingsDlg, &SettingsDlg::scaleChanged,
	    [this](EmojiFont::Scale scale){
		d_ptr->settings.setValue("scale", scale);
	    });
    connect(&d_ptr->settingsDlg, &SettingsDlg::waitMsChanged,
	    [this](int ms){
		XKeyInjector_setWaitMs(ms);
		d_ptr->settings.setValue("wait", ms);
	    });
}

void QXmoji::show()
{
    Q_D(QXmoji);
    QVariant size = d->settings.value("size");
    if (size.isValid())
    {
	d->win.resize(size.toSize());
    }
    d->win.show();
    XKeyInjector_setGrabWindow(d->win.effectiveWinId());
}

QXmoji::~QXmoji() {}
