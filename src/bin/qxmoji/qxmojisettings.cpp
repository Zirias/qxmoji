#include "qxmojisettings.h"

#include "nfsdetect.h"

#include <QDateTime>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QSize>
#include <QString>
#include <QTimer>

class QXmojiSettingsPrivate
{
    Q_DISABLE_COPY(QXmojiSettingsPrivate)
    Q_DECLARE_PUBLIC(QXmojiSettings)
    QXmojiSettings *const q_ptr;

    QString file;
    QSettings settings;
    QFileSystemWatcher watcher;
    QTimer timer;
    QDateTime lastModified;
    QString history;
    QSize size;
    EmojiFont::Scale scale;
    QXmoji::TrayMode trayMode;
    int wait;
    bool shown;
    bool singleInstance;
    bool nfs;

    QXmojiSettingsPrivate(QXmojiSettings *qxms, const QString &filename);

    void restore();
    void syncAndWatch(bool updateTimestamp = true);
    void set(const QString &key, const QVariant &value);
    void updateFromFile(int tries);
};

#define K_HISTORY QStringLiteral("history")
#define K_SCALE QStringLiteral("scale")
#define K_SHOWN QStringLiteral("shown")
#define K_SINGLEINSTANCE QStringLiteral("singleInstance")
#define K_SIZE QStringLiteral("size")
#define K_TRAYMODE QStringLiteral("trayMode")
#define K_WAIT QStringLiteral("wait")

QXmojiSettingsPrivate::QXmojiSettingsPrivate(
	QXmojiSettings *qxms, const QString &filename) :
    q_ptr(qxms),
    file(filename),
    settings(file, QSettings::IniFormat),
    watcher(),
    timer(),
    lastModified(QDateTime::currentDateTime()),
    history(settings.value(K_HISTORY).toString()),
    size(settings.value(K_SIZE, QSize(600, 240)).toSize()),
    scale(settings.value(K_SCALE, EmojiFont::Scale::Small)
	    .value<EmojiFont::Scale>()),
    trayMode(settings.value(K_TRAYMODE, QXmoji::TrayMode::Enabled)
	    .value<QXmoji::TrayMode>()),
    wait(settings.value(K_WAIT, 50).toInt()),
    shown(settings.value(K_SHOWN, true).toBool()),
    singleInstance(settings.value(K_SINGLEINSTANCE, true).toBool()),
    nfs(isNfs(qPrintable(file)))
{
    if (wait < 0) wait = 0;
    if (wait > 500) wait = 500;
    if (!QFileInfo::exists(file)) restore();
    else syncAndWatch();
}

void QXmojiSettingsPrivate::restore()
{
    settings.setValue(K_HISTORY, history);
    settings.setValue(K_SCALE, scale);
    settings.setValue(K_SHOWN, shown);
    settings.setValue(K_SINGLEINSTANCE, singleInstance);
    settings.setValue(K_SIZE, size);
    settings.setValue(K_TRAYMODE, trayMode);
    settings.setValue(K_WAIT, wait);
    syncAndWatch();
}

void QXmojiSettingsPrivate::syncAndWatch(bool updateTimestamp)
{
    settings.sync();
    if (nfs)
    {
	timer.start(1000);
	if (updateTimestamp)
	{
	    QFileInfo info(file);
	    QDateTime mod = info.lastModified();
	    if (mod.isValid()) lastModified = mod;
	}
    }
    else watcher.addPath(file);
}

void QXmojiSettingsPrivate::set(const QString &key, const QVariant &value)
{
    if (nfs) timer.stop();
    else watcher.removePath(file);
    settings.setValue(key, value);
    syncAndWatch();
}

void QXmojiSettingsPrivate::updateFromFile(int tries)
{
    Q_Q(QXmojiSettings);

    if (!QFileInfo::exists(file))
    {
	if (tries)
	{
	    QTimer::singleShot(100, q, [this, tries](){
		    updateFromFile(tries - 1); });
	}
	else restore();
	return;
    }
    if (nfs)
    {
	QFileInfo info(file);
	QDateTime mod = info.lastModified();
	if (!mod.isValid() || mod == lastModified) return;
	lastModified = mod;
    }
    syncAndWatch(false);

    QString nhistory = settings.value(K_HISTORY).toString();
    QSize nsize = settings.value(K_SIZE, QSize(600, 240)).toSize();
    EmojiFont::Scale nscale = settings.value(K_SCALE,
	    EmojiFont::Scale::Small).value<EmojiFont::Scale>();
    QXmoji::TrayMode ntrayMode = settings.value(K_TRAYMODE,
	    QXmoji::TrayMode::Enabled).value<QXmoji::TrayMode>();
    int nwait = settings.value(K_WAIT, 50).toInt();
    bool nshown = settings.value(K_SHOWN, true).toBool();
    bool nsingleInstance = settings.value(K_SINGLEINSTANCE, true).toBool();

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
    if (d_ptr->nfs) connect(&d_ptr->timer, &QTimer::timeout, [this](){
	    d_ptr->updateFromFile(5); });
    else connect(&d_ptr->watcher, &QFileSystemWatcher::fileChanged, [this](){
	    d_ptr->updateFromFile(5); });
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
    d->set(K_HISTORY, d->history);
    emit historyChanged(d->history);
}

void QXmojiSettings::setScale(EmojiFont::Scale scale)
{
    Q_D(QXmojiSettings);
    if (d->scale == scale) return;
    d->scale = scale;
    d->set(K_SCALE, d->scale);
    emit scaleChanged(d->scale);
}

void QXmojiSettings::setShown(bool shown)
{
    Q_D(QXmojiSettings);
    if (d->shown == shown) return;
    d->shown = shown;
    d->set(K_SHOWN, d->shown);
    emit shownChanged(d->shown);
}

void QXmojiSettings::setSingleInstance(bool singleInstance)
{
    Q_D(QXmojiSettings);
    if (d->singleInstance == singleInstance) return;
    d->singleInstance = singleInstance;
    d->set(K_SINGLEINSTANCE, d->singleInstance);
    emit singleInstanceChanged(d->singleInstance);
}

void QXmojiSettings::setSize(const QSize &size)
{
    Q_D(QXmojiSettings);
    if (d->size == size) return;
    d->size = size;
    d->set(K_SIZE, d->size);
    emit sizeChanged(d->size);
}

void QXmojiSettings::setTrayMode(QXmoji::TrayMode trayMode)
{
    Q_D(QXmojiSettings);
    if (d->trayMode == trayMode) return;
    d->trayMode = trayMode;
    d->set(K_TRAYMODE, d->trayMode);
    emit trayModeChanged(d->trayMode);
}

void QXmojiSettings::setWait(int wait)
{
    Q_D(QXmojiSettings);
    if (wait < 0) wait = 0;
    if (wait > 500) wait = 500;
    if (d->wait == wait) return;
    d->wait = wait;
    d->set(K_WAIT, d->wait);
    emit waitChanged(d->wait);
}

#include "moc_qxmojisettings.cpp"
