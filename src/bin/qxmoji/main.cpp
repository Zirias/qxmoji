#include "qxmoji.h"

#include "decl.h"

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
{
    QCoreApplication::setApplicationName("qXmoji");
    QCoreApplication::setApplicationVersion(QXMOJIVERSTR);

    QXmoji qxmoji(argc, argv);
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
    kill(getppid(), SIGHUP);

    qxmoji.show();
    return qxmoji.exec();
}

