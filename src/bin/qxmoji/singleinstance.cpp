#define _XOPEN_SOURCE 600

#include "singleinstance.h"

#include <pwd.h>
#include <unistd.h>

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QLocalServer>
#include <QLocalSocket>

class SingleInstancePrivate
{
    Q_DISABLE_COPY(SingleInstancePrivate)
    Q_DECLARE_PUBLIC(SingleInstance)
    SingleInstance *const q_ptr;

    const QString serverName;
    QLocalServer server;

    SingleInstancePrivate(SingleInstance *inst);
};

static QString initServerName()
{
    QCryptographicHash appData(QCryptographicHash::Sha256);
    appData.addData(QCoreApplication::applicationName().toUtf8());
    QByteArray username;
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) username = pw->pw_name;
    if (username.isEmpty()) username = qgetenv("USER");
    appData.addData(username);
    return appData.result().toBase64().replace("/","_");
}

SingleInstancePrivate::SingleInstancePrivate(SingleInstance *inst) :
    q_ptr(inst),
    serverName(initServerName())
{}

SingleInstance::SingleInstance() :
    d_ptr(new SingleInstancePrivate(this))
{
    connect(&d_ptr->server, &QLocalServer::newConnection, [this](){
		d_ptr->server.nextPendingConnection()->close();
		emit secondaryInstance();
	    });
}

SingleInstance::~SingleInstance() {}

bool SingleInstance::startInstance()
{
    Q_D(SingleInstance);

    bool listening = d->server.listen(d->serverName);
    if (!listening)
    {
	QLocalSocket client;
	client.connectToServer(d->serverName, QIODevice::WriteOnly);
	if (client.state() != QLocalSocket::ConnectedState &&
		!client.waitForConnected(2000))
	{
	    QLocalServer::removeServer(d->serverName);
	    listening = d->server.listen(d->serverName);
	}
	else client.disconnectFromServer();
    }
    return listening;
}

void SingleInstance::stopInstance()
{
    Q_D(SingleInstance);
    d->server.close();
}

#include "moc_singleinstance.cpp"
