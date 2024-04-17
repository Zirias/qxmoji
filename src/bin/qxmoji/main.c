#include "guimain.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int detach = 1;

static void sighdl(int sig)
{
    (void)sig;
}

static void started(void)
{
    if (detach)
    {
	int nulldev = open("/dev/null", O_RDWR);
	if (nulldev >= 0)
	{
	    dup2(nulldev, STDIN_FILENO);
	    dup2(nulldev, STDOUT_FILENO);
	    dup2(nulldev, STDERR_FILENO);
	    close(nulldev);
	}
	kill(getppid(), SIGHUP);
    }
}

static void dodetach(void)
{
    if (!detach) return;
    sigset_t sigset, oset;
    sigemptyset(&sigset);
    sigemptyset(&oset);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGHUP);
    sigaddset(&sigset, SIGCHLD);
    signal(SIGCHLD, sighdl);
    if (sigprocmask(SIG_BLOCK, &sigset, &oset) < 0)
    {
	fputs("Error: failed to block signals.\n", stderr);
	exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
	fputs("Error: failed to fork() into background.\n", stderr);
	exit(EXIT_FAILURE);
    }
    if (pid)
    {
	int signal;
	for (;;)
	{
	    sigwait(&sigset, &signal);
	    switch (signal)
	    {
		case SIGTERM:
		    fputs("Error: terminated during startup.\n", stderr);
		    exit(EXIT_FAILURE);

		case SIGHUP:
		    exit(EXIT_SUCCESS);

		case SIGCHLD:
		    if (waitpid(pid, &signal, WNOHANG) != pid)
		    {
			fputs("Warning: spurious SIGCHLD received.\n", stderr);
		    }
		    else if (WIFEXITED(signal))
		    {
			int rc = WEXITSTATUS(signal);
			if (rc != 0) fputs(
				"Error: failed to start.\n", stderr);
			exit(rc);
		    }
		    else if (WIFSIGNALED(signal))
		    {
			fprintf(stderr, "Error: terminated by signal %d.\n",
				WTERMSIG(signal));
			exit(EXIT_FAILURE);
		    }

		default:
		    ;
	    }
	}
    }
    else
    {
	signal(SIGCHLD, SIG_DFL);
	if (sigprocmask(SIG_SETMASK, &oset, 0) < 0)
	{
	    fputs("Error: failed to restore signal mask.\n", stderr);
	    exit(EXIT_FAILURE);
	}
	setsid();
    }
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

    dodetach();
    return guimain(argc, argv, started);
}

