#include "qxmoji.h"

#include "aboutdlg.h"
#include "emojibutton.h"
#include "emojifont.h"
#include "emojihistory.h"
#include "qxmojiwin.h"
#include "settingsdlg.h"
#include "singleinstance.h"
#include "xcbadapter.h"
#include "xkeyinjector.h"

#include <QAction>
#include <QMenu>
#include <QDir>
#include <QIcon>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QWindow>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QX11Info>
#endif

class QXmojiPrivate {
    Q_DISABLE_COPY(QXmojiPrivate)
    Q_DECLARE_PUBLIC(QXmoji)
    QXmoji *const q_ptr;

    XcbAdapter *xcb;
    EmojiFont font;
    SingleInstance instance;
    QAction aboutAct;
    QAction settingsAct;
    QAction exitAct;
    QMenu contextMenu;
    QIcon appIcon;
    QSystemTrayIcon trayIcon;
    QSettings settings;
    QXmojiWin win;
    AboutDlg aboutDlg;
    SettingsDlg settingsDlg;
    QXmoji::TrayMode mode;
    int waitms;
    bool startOk;
    
    QXmojiPrivate(QXmoji *);

public:
    ~QXmojiPrivate();
};

QXmojiPrivate::QXmojiPrivate(QXmoji *app) :
    q_ptr(app),
    xcb(0),
    aboutAct("&About"),
    settingsAct("&Settings"),
    exitAct("E&xit"),
    settings(QDir::homePath() + "/.config/qxmoji.ini", QSettings::IniFormat),
    win(&contextMenu, &font),
    aboutDlg(&win),
    settingsDlg(&win),
    startOk(false)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!QX11Info::isPlatformX11()) return;
    xcb = XcbAdapter_create(QX11Info::connection());
#else
    auto x11app = app->nativeInterface<QNativeInterface::QX11Application>();
    if (!x11app) return;
    xcb = XcbAdapter_create(x11app->connection());
#endif

    bool singleInstance = settings.value("singleInstance", true).toBool();
    if (singleInstance)
    {
	if (!instance.startInstance()) return;
    }

    startOk = true;

    appIcon.addPixmap(QPixmap(":/icon_48.png"));
    appIcon.addPixmap(QPixmap(":/icon_32.png"));
    appIcon.addPixmap(QPixmap(":/icon_16.png"));
    win.setWindowIcon(appIcon);
    aboutDlg.setWindowIcon(appIcon);
    settingsDlg.setWindowIcon(appIcon);
    settingsDlg.setSingleInstance(singleInstance);
    trayIcon.setIcon(appIcon);
    trayIcon.setToolTip("Emoji keyboard");
    trayIcon.setContextMenu(&contextMenu);
    mode = settings.value("trayMode",
	    QXmoji::TrayMode::Enabled).value<QXmoji::TrayMode>();
    settingsDlg.setTrayMode(mode);
    EmojiFont::Scale scale = settings.value(
	    "scale", EmojiFont::Scale::Small).value<EmojiFont::Scale>();
    font.setScale(scale);
    settingsDlg.setScale(scale);
    waitms = settings.value("wait", 50).toInt();
    if (waitms < 0) waitms = 0;
    if (waitms > 500) waitms = 500;
    settingsDlg.setWaitMs(waitms);
    QByteArray history = settings.value("history", QByteArray()).toByteArray();
    Emoji_loadHistory(history.length(), history);
    contextMenu.addAction(&aboutAct);
    contextMenu.addAction(&settingsAct);
    contextMenu.addAction(&exitAct);
}

QXmojiPrivate::~QXmojiPrivate()
{
    XcbAdapter_destroy(xcb);
}

QXmoji::QXmoji(int &argc, char **argv) :
    QApplication(argc, argv),
    d_ptr(new QXmojiPrivate(this))
{
    if (!d_ptr->startOk) return;

    setQuitOnLastWindowClosed(false);
    if (d_ptr->mode != QXmoji::TrayMode::Disabled)
    {
	d_ptr->trayIcon.show();
	if (QSystemTrayIcon::isSystemTrayAvailable())
	{
	    d_ptr->win.setHideInTaskbar(true);
	}
    }
    if (d_ptr->mode == QXmoji::TrayMode::Minimize)
    {
	d_ptr->win.setCloseOnMinimize(true);
    }

    connect(&d_ptr->win, &QXmojiWin::clicked, [this](const EmojiButton *b){
	    XKeyInjector_inject(d_ptr->xcb, b->emoji(), d_ptr->waitms); });
    connect(&d_ptr->aboutAct, &QAction::triggered,
	    &d_ptr->aboutDlg, &QWidget::show);
    connect(&d_ptr->settingsAct, &QAction::triggered,
	    &d_ptr->settingsDlg, &QWidget::show);
    connect(&d_ptr->exitAct, &QAction::triggered, qApp,
	    &QCoreApplication::quit, Qt::QueuedConnection);
    connect(&d_ptr->win, &QXmojiWin::closing,
	    [this](bool minimize){
		(void)minimize;
		d_ptr->settings.setValue("size", d_ptr->win.size());
		size_t historysz;
		const void *historybytes = Emoji_saveHistory(&historysz);
		QByteArray history((const char *)historybytes, historysz);
		d_ptr->settings.setValue("history", history);
	    });
    connect(&d_ptr->win, &QXmojiWin::closed,
	    [this](bool minimize){
		bool haveTray = QSystemTrayIcon::isSystemTrayAvailable();
		switch (d_ptr->mode)
		{
		    case TrayMode::Disabled:
			quit();
			break;

		    case TrayMode::Enabled:
			if (!haveTray) quit();
			break;

		    case TrayMode::Minimize:
			if (!minimize || !haveTray) quit();
			break;
		}
	    });
    connect(&d_ptr->settingsDlg, &SettingsDlg::singleInstanceChanged,
	    [this](bool singleInstance){
		d_ptr->settings.setValue("singleInstance", singleInstance);
	    });
    connect(&d_ptr->settingsDlg, &SettingsDlg::trayModeChanged,
	    [this](TrayMode mode){
		d_ptr->mode = mode;
		d_ptr->settings.setValue("trayMode", mode);
		if (mode == TrayMode::Disabled)
		{
		    d_ptr->trayIcon.hide();
		    d_ptr->win.show();
		}
		else d_ptr->trayIcon.show();
		d_ptr->win.setCloseOnMinimize(mode == TrayMode::Minimize);
		d_ptr->win.setHideInTaskbar(mode != TrayMode::Disabled
			&& QSystemTrayIcon::isSystemTrayAvailable());
	    });
    connect(&d_ptr->settingsDlg, &SettingsDlg::scaleChanged,
	    &d_ptr->font, &EmojiFont::setScale);
    connect(&d_ptr->settingsDlg, &SettingsDlg::scaleChanged,
	    [this](EmojiFont::Scale scale){
		d_ptr->settings.setValue("scale", scale);
	    });
    connect(&d_ptr->settingsDlg, &SettingsDlg::waitMsChanged,
	    [this](int ms){
		d_ptr->waitms = ms;
		d_ptr->settings.setValue("wait", ms);
	    });
    auto showandraise = [this](){
	    if (!d_ptr->win.isVisible()) d_ptr->win.show();
	    d_ptr->win.setWindowState(d_ptr->win.windowState()
		    & ~Qt::WindowMinimized);
	    d_ptr->win.raise();
    };
    connect(&d_ptr->instance, &SingleInstance::secondaryInstance,
	    showandraise);
    connect(&d_ptr->trayIcon, &QSystemTrayIcon::activated,
	    [showandraise](QSystemTrayIcon::ActivationReason reason){
		if (reason == QSystemTrayIcon::Trigger) showandraise();
	    });
}

QXmoji::~QXmoji() {}

bool QXmoji::startOk()
{
    Q_D(QXmoji);
    return d->startOk;
}

XcbAdapter *QXmoji::xcb()
{
    Q_D(QXmoji);
    return d->xcb;
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
}
