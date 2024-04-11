#include "singleinstance.h"

#include <pwd.h>
#include <unistd.h>

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QLocalServer>
#include <QLocalSocket>

SingleInstance::SingleInstance() : QObject() {}

bool SingleInstance::startInstance()
{
    QCryptographicHash appData(QCryptographicHash::Sha256);
    appData.addData(QCoreApplication::applicationName().toUtf8());
    QByteArray username;
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) username = pw->pw_name;
    if (username.isEmpty()) username = qgetenv("USER");
    appData.addData(username);
    QString serverName = appData.result().toBase64().replace("/","_");

    QLocalServer *server = new QLocalServer(this);
    connect(server, &QLocalServer::newConnection, [this, server](){
		server->nextPendingConnection()->close();
		emit secondaryInstance();
	    });

    bool listening = server->listen(serverName);
    if (!listening)
    {
	QLocalSocket client;
	client.connectToServer(serverName, QIODevice::WriteOnly);
	if (client.state() != QLocalSocket::ConnectedState &&
		!client.waitForConnected(2000))
	{
	    QLocalServer::removeServer(serverName);
	    listening = server->listen(serverName);
	}
	else client.disconnectFromServer();
    }
    return listening;
}
