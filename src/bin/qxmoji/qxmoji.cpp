#include "qxmoji.h"

#include "aboutdlg.h"
#include "emojibutton.h"
#include "emojifont.h"
#include "emojihistory.h"
#include "qxmojisettings.h"
#include "qxmojiwin.h"
#include "settingsdlg.h"
#include "singleinstance.h"
#include "xcbadapter.h"
#include "xkeyinjector.h"

#include <QAction>
#include <QMenu>
#include <QDir>
#include <QIcon>
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
    QAction showAct;
    QAction aboutAct;
    QAction settingsAct;
    QAction exitAct;
    QMenu contextMenu;
    QIcon appIcon;
    QSystemTrayIcon trayIcon;
    QXmojiSettings settings;
    EmojiHistory history;
    QXmojiWin win;
    AboutDlg aboutDlg;
    SettingsDlg settingsDlg;
    bool exiting;
    
    QXmojiPrivate(QXmoji *);

public:
    ~QXmojiPrivate();
};

QXmojiPrivate::QXmojiPrivate(QXmoji *app) :
    q_ptr(app),
    xcb(0),
    showAct("Show &Window"),
    aboutAct("&About"),
    settingsAct("&Settings"),
    exitAct("E&xit"),
    settings(QDir::homePath() + "/.config/qxmoji.ini"),
    history(settings.history()),
    win(&contextMenu, &font, &history),
    aboutDlg(&win),
    settingsDlg(&win, &settings),
    exiting(false)
{
    int rc = -1;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!QX11Info::isPlatformX11()) rc = 1;
    else xcb = XcbAdapter_create(QX11Info::connection());
#else
    auto x11app = app->nativeInterface<QNativeInterface::QX11Application>();
    if (!x11app) rc = 1;
    else xcb = XcbAdapter_create(x11app->connection());
#endif

    if (rc < 0 && settings.singleInstance()
	    && !instance.startInstance()) rc = 0;
    if (rc >= 0)
    {
	XcbAdapter_destroy(xcb);
	xcb = 0;
	QMetaObject::invokeMethod(app, [rc](){ QCoreApplication::exit(rc); },
		Qt::QueuedConnection);
	return;
    }

    appIcon.addPixmap(QPixmap(":/icon_48.png"));
    appIcon.addPixmap(QPixmap(":/icon_32.png"));
    appIcon.addPixmap(QPixmap(":/icon_16.png"));
    win.setWindowIcon(appIcon);
    aboutDlg.setWindowIcon(appIcon);
    settingsDlg.setWindowIcon(appIcon);
    trayIcon.setIcon(appIcon);
    trayIcon.setToolTip("Emoji keyboard");
    trayIcon.setContextMenu(&contextMenu);
    font.setScale(settings.scale());
    contextMenu.addAction(&showAct);
    contextMenu.addSeparator();
    contextMenu.addAction(&aboutAct);
    contextMenu.addAction(&settingsAct);
    contextMenu.addAction(&exitAct);
}

QXmojiPrivate::~QXmojiPrivate()
{
    XcbAdapter_destroy(xcb);
}

QXmoji::QXmoji(int &argc, char **argv, void (*started)()) :
    QApplication(argc, argv),
    d_ptr(new QXmojiPrivate(this))
{
    if (!d_ptr->xcb) return;
    started();

    setQuitOnLastWindowClosed(false);
    if (d_ptr->settings.trayMode() != QXmoji::TrayMode::Disabled)
    {
	d_ptr->trayIcon.show();
	if (QSystemTrayIcon::isSystemTrayAvailable())
	{
	    d_ptr->win.setHideInTaskbar(true);
	}
    }
    if (d_ptr->settings.trayMode() == QXmoji::TrayMode::Minimize)
    {
	d_ptr->win.setCloseOnMinimize(true);
    }

    connect(&d_ptr->win, &QXmojiWin::emojiSelected,
	    [this](EmojiArg *emoji){
		XKeyInjector_inject(d_ptr->xcb, EMOJI(emoji),
			d_ptr->settings.wait());
		d_ptr->history.record(EMOJI(emoji));
	    });
    connect(&d_ptr->history, &EmojiHistory::changed, [this](){
	    d_ptr->settings.setHistory(d_ptr->history.serialize()); });
    connect(&d_ptr->aboutAct, &QAction::triggered,
	    &d_ptr->aboutDlg, &QWidget::show);
    connect(&d_ptr->settingsAct, &QAction::triggered,
	    &d_ptr->settingsDlg, &QWidget::show);
    connect(&d_ptr->exitAct, &QAction::triggered,
	    [this]() {
		d_ptr->exiting = true;
		QMetaObject::invokeMethod(this,
			&QCoreApplication::quit, Qt::QueuedConnection);
	    });
    connect(&d_ptr->win, &QXmojiWin::closing, [this](){
	    d_ptr->settings.setSize(d_ptr->win.size()); });
    connect(&d_ptr->win, &QXmojiWin::closed,
	    [this](bool minimize){
		d_ptr->showAct.setVisible(true);
		bool haveTray = QSystemTrayIcon::isSystemTrayAvailable();
		switch (d_ptr->settings.trayMode())
		{
		    case TrayMode::Disabled:
			quit();
			break;

		    case TrayMode::Enabled:
			if (!haveTray) quit();
			else if (!d_ptr->exiting)
			{
			    d_ptr->settings.setShown(false);
			}
			break;

		    case TrayMode::Minimize:
			if (!minimize || !haveTray) quit();
			else if (!d_ptr->exiting)
			{
			    d_ptr->settings.setShown(false);
			}
			break;
		}
	    });
    connect(&d_ptr->settings, &QXmojiSettings::historyChangedExt,
	    [this](const QString &h){ d_ptr->history.load(h); });
    connect(&d_ptr->settings, &QXmojiSettings::trayModeChanged,
	    [this](TrayMode mode){
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
    connect(&d_ptr->settings, &QXmojiSettings::singleInstanceChanged,
	    [this](bool single){
		if (single)
		{
		    if (!d_ptr->instance.startInstance()) quit();
		}
		else d_ptr->instance.stopInstance();
	    });
    connect(&d_ptr->settings, &QXmojiSettings::scaleChanged,
	    &d_ptr->font, &EmojiFont::setScale);
    auto showandraise = [this](){
	    d_ptr->settings.setShown(true);
	    d_ptr->showAct.setVisible(false);
	    if (!d_ptr->win.isVisible()) d_ptr->win.show();
	    d_ptr->win.setWindowState(d_ptr->win.windowState()
		    & ~Qt::WindowMinimized);
	    d_ptr->win.raise();
    };
    connect(&d_ptr->showAct, &QAction::triggered, showandraise);
    connect(&d_ptr->instance, &SingleInstance::secondaryInstance,
	    showandraise);
    connect(&d_ptr->trayIcon, &QSystemTrayIcon::activated,
	    [showandraise](QSystemTrayIcon::ActivationReason reason){
		if (reason == QSystemTrayIcon::Trigger) showandraise();
	    });

    d_ptr->win.resize(d_ptr->settings.size());
    if (d_ptr->settings.shown()
	    || d_ptr->settings.trayMode() == TrayMode::Disabled
	    || !QSystemTrayIcon::isSystemTrayAvailable())
    {
	d_ptr->settings.setShown(true);
	d_ptr->showAct.setVisible(false);
	d_ptr->win.show();
    }
}

QXmoji::~QXmoji() {}

XcbAdapter *QXmoji::xcb()
{
    Q_D(QXmoji);
    return d->xcb;
}

#include "moc_qxmoji.cpp"
