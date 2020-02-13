#include "matt_daemon.hpp"

static int	check_credentials(void)
{
	uid_t cred;

	if ((cred = geteuid()) == 0)
		return (EXIT_SUCCESS);
	std::cout << "Error: not root credentials" << std::endl;
	return (EXIT_FAILURE);
}

static int	lock_daemon(int *fd)
{
	*fd = open("/var/lock/matt_daemon.lock", O_WRONLY | O_CREAT);
	if (*fd > 0)

		if ((flock(*fd, LOCK_EX | LOCK_NB)) < 0)
		{
			close(*fd);
			goto error;
		}
	return (EXIT_SUCCESS);
error:
	std::cout << "Error: Matt_daemon already run" << std::endl;
	return (EXIT_FAILURE);
}

static void	unlock_deamon(int *fd)
{
	flock(*fd, LOCK_UN);
	close(*fd);
	*fd = 0;
}

static void	quit(int *fd)
{
	unlock_deamon(fd);
	exit (EXIT_SUCCESS);
}

void		sig_handler(int signo)
{
	(void)signo;
	std::cout << "SIG HANDLER" << std::endl;
	exit(EXIT_SUCCESS);
}

int		main(void)
{
	pid_t	pid;
	pid_t	sid;
	int	fd;

	fd = 0;
	if (check_credentials())
		return (EXIT_FAILURE);
	//lock
	if (lock_daemon(&fd))
		return (EXIT_FAILURE);
	pid = fork();
	if (pid < 0)
		return (EXIT_FAILURE);
	if (pid == 0)
	{
		//catch sig
		signal(SIGTSTP, SIG_IGN);
		signal(SIGCONT, SIG_IGN);
		signal(SIGTTIN, SIG_IGN);
		signal(SIGTTOU, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
		signal(SIGCHLD, SIG_DFL);
		signal(SIGTERM, sig_handler);
		signal(SIGINT, sig_handler);
		signal(SIGQUIT, sig_handler);

		//change umask
		umask(0);

		//TODO:create log

		//create group
		sid = setsid();
		if (sid < 0)
			return (EXIT_FAILURE);

		//change working dir
		if ((chdir("/") < 0))
			return (EXIT_FAILURE);

		//close out std f
		//close(STDIN_FILENO);
		//close(STDOUT_FILENO);
		//close(STDERR_FILENO);
		while (1);
		quit(&fd);

	}
	return (EXIT_SUCCESS);
}
