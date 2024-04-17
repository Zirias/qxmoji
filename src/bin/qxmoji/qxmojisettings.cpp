#include "qxmojisettings.h"

#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QSize>
#include <QString>

class QXmojiSettingsPrivate
{
    Q_DISABLE_COPY(QXmojiSettingsPrivate)
    Q_DECLARE_PUBLIC(QXmojiSettings)
    QXmojiSettings *const q_ptr;

    QFileInfo file;
    QSettings settings;
    QFileSystemWatcher watcher;
    QString history;
    QSize size;
    EmojiFont::Scale scale;
    QXmoji::TrayMode trayMode;
    int wait;
    bool shown;
    bool singleInstance;

    QXmojiSettingsPrivate(QXmojiSettings *qxms, const QString &file);

    void unwatch();
    void syncAndWatch();
    void updateFromFile();
};

#define K_HISTORY QStringLiteral("history")
#define K_SCALE QStringLiteral("scale")
#define K_SHOWN QStringLiteral("shown")
#define K_SINGLEINSTANCE QStringLiteral("singleInstance")
#define K_SIZE QStringLiteral("size")
#define K_TRAYMODE QStringLiteral("trayMode")
#define K_WAIT QStringLiteral("wait")

QXmojiSettingsPrivate::QXmojiSettingsPrivate(
	QXmojiSettings *qxms, const QString &file) :
    q_ptr(qxms),
    file(file),
    settings(file, QSettings::IniFormat),
    watcher(),
    history(settings.value(K_HISTORY).toString()),
    size(settings.value(K_SIZE, QSize(600, 240)).toSize()),
    scale(settings.value(K_SCALE, EmojiFont::Scale::Small)
	    .value<EmojiFont::Scale>()),
    trayMode(settings.value(K_TRAYMODE, QXmoji::TrayMode::Enabled)
	    .value<QXmoji::TrayMode>()),
    wait(settings.value(K_WAIT, 50).toInt()),
    shown(settings.value(K_SHOWN, true).toBool()),
    singleInstance(settings.value(K_SINGLEINSTANCE, true).toBool())
{
    if (wait < 0) wait = 0;
    if (wait > 500) wait = 500;
    syncAndWatch();
}

void QXmojiSettingsPrivate::unwatch()
{
    watcher.removePath(file.filePath());
}

void QXmojiSettingsPrivate::syncAndWatch()
{
    if (!file.exists())
    {
	settings.setValue(K_HISTORY, history);
	settings.setValue(K_SCALE, scale);
	settings.setValue(K_SHOWN, shown);
	settings.setValue(K_SINGLEINSTANCE, singleInstance);
	settings.setValue(K_SIZE, size);
	settings.setValue(K_TRAYMODE, trayMode);
	settings.setValue(K_WAIT, wait);
    }
    settings.sync();
    watcher.addPath(file.filePath());
}

void QXmojiSettingsPrivate::updateFromFile()
{
    Q_Q(QXmojiSettings);

    settings.sync();
    QString nhistory = settings.value(K_HISTORY).toString();
    QSize nsize = settings.value(K_SIZE, QSize(600, 240)).toSize();
    EmojiFont::Scale nscale = settings.value(K_SCALE,
	    EmojiFont::Scale::Small).value<EmojiFont::Scale>();
    QXmoji::TrayMode ntrayMode = settings.value(K_TRAYMODE,
	    QXmoji::TrayMode::Enabled).value<QXmoji::TrayMode>();
    int nwait = settings.value(K_WAIT, 50).toInt();
    bool nshown = settings.value(K_SHOWN, true).toBool();
    bool nsingleInstance = settings.value(K_SINGLEINSTANCE, true).toBool();
    watcher.addPath(file.filePath());

    if (nhistory != history)
    {
	history = nhistory;
	emit q->historyChangedExt(history);
	emit q->historyChanged(history);
    }
    if (nsize != size)
    {
	size = nsize;
	emit q->sizeChangedExt(size);
	emit q->sizeChanged(size);
    }
    if (nscale != scale)
    {
	scale = nscale;
	emit q->scaleChangedExt(scale);
	emit q->scaleChanged(scale);
    }
    if (ntrayMode != trayMode)
    {
	trayMode = ntrayMode;
	emit q->trayModeChangedExt(trayMode);
	emit q->trayModeChanged(trayMode);
    }
    if (nwait != wait)
    {
	wait = nwait;
	if (wait < 0) wait = 0;
	if (wait > 500) wait = 500;
	emit q->waitChangedExt(wait);
	emit q->waitChanged(wait);
    }
    if (nshown != shown)
    {
	shown = nshown;
	emit q->shownChangedExt(shown);
	emit q->shownChanged(shown);
    }
    if (nsingleInstance != singleInstance)
    {
	singleInstance = nsingleInstance;
	emit q->singleInstanceChangedExt(singleInstance);
	emit q->singleInstanceChanged(singleInstance);
    }
}

QXmojiSettings::QXmojiSettings(const QString &file) :
    d_ptr(new QXmojiSettingsPrivate(this, file))
{
    connect(&d_ptr->watcher, &QFileSystemWatcher::fileChanged,
	    [this](){ d_ptr->updateFromFile(); });
}

QXmojiSettings::~QXmojiSettings() {}

const QString &QXmojiSettings::history() const
{
    Q_D(const QXmojiSettings);
    return d->history;
}

EmojiFont::Scale QXmojiSettings::scale() const
{
    Q_D(const QXmojiSettings);
    return d->scale;
}

bool QXmojiSettings::shown() const
{
    Q_D(const QXmojiSettings);
    return d->shown;
}

bool QXmojiSettings::singleInstance() const
{
    Q_D(const QXmojiSettings);
    return d->singleInstance;
}

const QSize &QXmojiSettings::size() const
{
    Q_D(const QXmojiSettings);
    return d->size;
}

QXmoji::TrayMode QXmojiSettings::trayMode() const
{
    Q_D(const QXmojiSettings);
    return d->trayMode;
}

int QXmojiSettings::wait() const
{
    Q_D(const QXmojiSettings);
    return d->wait;
}

void QXmojiSettings::setHistory(const QString &history)
{
    Q_D(QXmojiSettings);
    if (d->history == history) return;
    d->history = history;
    d->unwatch();
    d->settings.setValue(K_HISTORY, d->history);
    d->syncAndWatch();
    emit historyChanged(d->history);
}

void QXmojiSettings::setScale(EmojiFont::Scale scale)
{
    Q_D(QXmojiSettings);
    if (d->scale == scale) return;
    d->scale = scale;
    d->unwatch();
    d->settings.setValue(K_SCALE, d->scale);
    d->syncAndWatch();
    emit scaleChanged(d->scale);
}

void QXmojiSettings::setShown(bool shown)
{
    Q_D(QXmojiSettings);
    if (d->shown == shown) return;
    d->shown = shown;
    d->unwatch();
    d->settings.setValue(K_SHOWN, d->shown);
    d->syncAndWatch();
    emit shownChanged(d->shown);
}

void QXmojiSettings::setSingleInstance(bool singleInstance)
{
    Q_D(QXmojiSettings);
    if (d->singleInstance == singleInstance) return;
    d->singleInstance = singleInstance;
    d->unwatch();
    d->settings.setValue(K_SINGLEINSTANCE, d->singleInstance);
    d->syncAndWatch();
    emit singleInstanceChanged(d->singleInstance);
}

void QXmojiSettings::setSize(const QSize &size)
{
    Q_D(QXmojiSettings);
    if (d->size == size) return;
    d->size = size;
    d->unwatch();
    d->settings.setValue(K_SIZE, d->size);
    d->syncAndWatch();
    emit sizeChanged(d->size);
}

void QXmojiSettings::setTrayMode(QXmoji::TrayMode trayMode)
{
    Q_D(QXmojiSettings);
    if (d->trayMode == trayMode) return;
    d->trayMode = trayMode;
    d->unwatch();
    d->settings.setValue(K_TRAYMODE, d->trayMode);
    d->syncAndWatch();
    emit trayModeChanged(d->trayMode);
}

void QXmojiSettings::setWait(int wait)
{
    Q_D(QXmojiSettings);
    if (wait < 0) wait = 0;
    if (wait > 500) wait = 500;
    if (d->wait == wait) return;
    d->wait = wait;
    d->unwatch();
    d->settings.setValue(K_WAIT, d->wait);
    d->syncAndWatch();
    emit waitChanged(d->wait);
}

