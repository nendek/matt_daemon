#include "matt_daemon.hpp"

static void	quit(int *fd, Tintin_reporter *log)
{
	unlock_deamon(fd);
	log->log(info, "Daemon stopped");
	delete log;
	exit (EXIT_SUCCESS);
}

int		main(void)
{
	pid_t		pid;
	pid_t		sid;
	int		fd;
	int		sock;


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

		//TODO try catch
		if (mkdir("/var/log/matt_daemon", 0444) < 0)
		{
			//TODO throw error
			std::cout << "mkdir" << std::endl;
		}
		Tintin_reporter* log = new Tintin_reporter("/var/log/matt_daemon/matt_daemon.log");

		sid = setsid();
		if (sid < 0)
		{
			log->log(error, "Error setsid()");
			return (EXIT_FAILURE);
		}
		if ((chdir("/") < 0))
		{
			log->log(error, "Error chdir()");
			return (EXIT_FAILURE);
		}

		//close out std f
		//close(STDIN_FILENO);
		//close(STDOUT_FILENO);
		//close(STDERR_FILENO);
		log->log(info, "Daemon create");
		if ((sock = create_server(log)) < 0)
			return (EXIT_FAILURE);
		log->log(info, "Server create");
		run_server(&sock, log);
		quit(&fd, log);
	}
	return (EXIT_SUCCESS);
}
