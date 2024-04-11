#include "guimain.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int detach = 1;

static void started(void)
{
    if (detach) kill(getppid(), SIGHUP);
}

int main(int argc, char **argv)
{
    char *platformenv = getenv("QT_QPA_PLATFORM");
    if (platformenv && strcmp(platformenv, "xcb"))
    {
	fprintf(stderr, "Warning: QT_QPA_PLATFORM is set to `%s'. "
		"qXmoji only works with the `xcb' platform, ignoring.\n",
		platformenv);
    }
    setenv("QT_QPA_PLATFORM", "xcb", 1);

    for (int i = 1; i < argc; ++i)
    {
	if (!strcmp(argv[i], "-platform"))
	{
	    if (++i < argc && strcmp(argv[i], "xcb"))
	    {
		fprintf(stderr, "Error: Invalid `-platform %s' given. "
			"qXmoji only works with the `xcb' platform.\n",
			argv[i]);
		return EXIT_FAILURE;
	    }
	}
	else if (!strcmp(argv[i], "-d")) detach = 0;
    }

    if (detach)
    {
	sigset_t sigset, oset;
	sigemptyset(&sigset);
	sigemptyset(&oset);
	sigaddset(&sigset, SIGTERM);
	sigaddset(&sigset, SIGHUP);
	sigaddset(&sigset, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &sigset, &oset) < 0)
	{
	    fputs("Error: failed to block signals.\n", stderr);
	    return EXIT_FAILURE;
	}

	pid_t pid = fork();
	if (pid < 0)
	{
	    fputs("Error: failed to fork() into background.\n", stderr);
	    return EXIT_FAILURE;
	}
	if (pid)
	{
	    int signal;
	    sigwait(&sigset, &signal);
	    return signal == SIGHUP ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	else if (sigprocmask(SIG_SETMASK, &oset, 0) < 0)
	{
	    fputs("Error: failed to restore signal mask.\n", stderr);
	    return EXIT_FAILURE;
	}
	setsid();
    }

    guimain(argc, argv, started);
}

