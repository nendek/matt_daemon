#include "matt_daemon.hpp"

Tintin_reporter*	sig_log;
int			sig_fd;
int			sig_sock;

static void		sig_handler(int signo)
{
	if (sig_fd != 0)
		unlock_deamon(&sig_fd);
	if (sig_sock != 0)
		close(sig_sock);
	if (sig_log != NULL)
	{
		sig_log->log(info, "Signal receive num:" + std::to_string(signo));
		sig_log->log(info, "Server shutdown");
		delete sig_log;
	}
	exit(EXIT_SUCCESS);
}

static void		sigsig(void)
{
		signal(SIGTSTP, SIG_IGN);
		signal(SIGCONT, SIG_IGN);
		signal(SIGTTIN, SIG_IGN);
		signal(SIGTTOU, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
		signal(SIGCHLD, SIG_DFL);
		signal(SIGTERM, sig_handler);
		signal(SIGINT, sig_handler);
		signal(SIGQUIT, sig_handler);
}

int			main(void)
{
	pid_t			pid;
	pid_t			sid;
	int			fd;
	int			sock;
	DIR			*dir;
	Tintin_reporter*	log;

	sig_log = NULL;
	sig_fd = 0;
	sig_sock = 0;
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
	sig_fd = fd;
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
		sig_log = log;
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		if ((sock = create_server(log)) < 0)
		{
			unlock_deamon(&fd);
			delete log;
			return (EXIT_FAILURE);
		}
		sig_sock = sock;
		log->log(info, "Server create");
		run_server(&sock, log);
		log->log(info, "Server shutdown");
		unlock_deamon(&fd);
		delete log;
	}
	return (EXIT_SUCCESS);
}
