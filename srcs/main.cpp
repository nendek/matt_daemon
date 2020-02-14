#include "matt_daemon.hpp"

static void	quit(int *fd)
{
	unlock_deamon(fd);
	exit (EXIT_SUCCESS);
}

int		main(void)
{
	pid_t	pid;
	pid_t	sid;
	int	fd;
	int	sock;

	fd = 0;
	if (check_credentials())
		return (EXIT_FAILURE);
	if (lock_daemon(&fd))
		return (EXIT_FAILURE);
	pid = fork();
	if (pid < 0)
		return (EXIT_FAILURE);
	if (pid == 0)
	{
		sigsig();
		umask(0);

		//TODO:create log

		sid = setsid();
		if (sid < 0)
			return (EXIT_FAILURE);
		if ((chdir("/") < 0))
			return (EXIT_FAILURE);

		//close out std f
		//close(STDIN_FILENO);
		//close(STDOUT_FILENO);
		//close(STDERR_FILENO);
		if ((sock = create_server()) < 0)
			return (EXIT_FAILURE);
		run_server(sock);
		quit(&fd);
	}
	return (EXIT_SUCCESS);
}
