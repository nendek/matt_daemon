#include "matt_daemon.hpp"

int		main(void)
{
	pid_t			pid;
	pid_t			sid;
	int			fd;
	int			sock;
	DIR			*dir;
	Tintin_reporter*	log;

	fd = 0;
	try
	{
		check_credentials();
		lock_daemon(&fd);
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	try
	{
		pid = fork();
		if (pid < 0)
			throw Error(errno, strerror(errno), 2);
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		unlock_deamon(&fd);
		return (EXIT_FAILURE);
	}
	if (pid == 0)
	{
		sigsig();
		umask(0);
		dir = opendir("/var/log/matt_daemon");
		try
		{
			if (!dir)
			{
				if (errno == ENOENT)
				{
					if (mkdir("/var/log/matt_daemon", 0444) < 0)
						throw Error(errno, strerror(errno), 2);
				}
				else
					throw Error(errno, strerror(errno), 2);
			}
		}
		catch (std::exception const& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
			unlock_deamon(&fd);
			closedir(dir);
			return (EXIT_FAILURE);
		}
		closedir(dir);
		try
		{
			sid = setsid();
			if (sid < 0)
				throw Error(errno, strerror(errno), 2);
			if ((chdir("/") < 0))
				throw Error(errno, strerror(errno), 2);
			log = new Tintin_reporter("/var/log/matt_daemon/matt_daemon.log");
		}
		catch (std::exception const& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
			unlock_deamon(&fd);
			return (EXIT_FAILURE);
		}
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		log->log(info, "Daemon create");
		if ((sock = create_server(log)) < 0)
		{
			unlock_deamon(&fd);
			delete log;
			return (EXIT_FAILURE);
		}
		log->log(info, "Server create");
		run_server(&sock, log);
		log->log(info, "Server shutdown");
		unlock_deamon(&fd);
		delete log;
	}
	return (EXIT_SUCCESS);
}
