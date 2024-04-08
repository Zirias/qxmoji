#include "qxmoji.h"

#include "xkeyinjector.h"

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <QX11Info>
#include <iostream>

#if NDEBUG
static int guimain(int argc, char **argv);

void handlesig(int sig)
{
    (void)sig;
    _exit(0);
}

int main(int argc, char **argv)
{
    pid_t pid = fork();
    if (pid < 0) return 1;
    if (pid)
    {
	signal(SIGHUP, handlesig);
	for (;;)
	{
	    int crc;
	    if (wait(&crc) > 0)
	    {
		if (WIFEXITED(crc)) return 0;
		else return WEXITSTATUS(crc);
	    }
	}
    }
    setsid();
    return guimain(argc, argv);
}

static int guimain(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    QCoreApplication::setApplicationName("qXmoji");
    QCoreApplication::setApplicationVersion(QXMOJIVERSTR);

    QXmoji qxmoji(argc, argv);

    if (!QX11Info::isPlatformX11())
    {
	std::cerr << "qXmoji requires X11" << std::endl;
	return 1;
    }
    XKeyInjector_setConnection(QX11Info::connection());

#if NDEBUG
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
    kill(getppid(), SIGHUP);
#endif

    qxmoji.show();
    return qxmoji.exec();
}

