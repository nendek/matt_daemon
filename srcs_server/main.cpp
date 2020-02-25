#include "matt_daemon.hpp"

Tintin_reporter*	g_log;
int			g_fd;
int			g_sock;

static void		sig_handler(const int signo)
{
	if (g_fd != 0)
		unlock_deamon(&g_fd);
	if (g_sock != 0)
		close(g_sock);
	if (g_log != NULL)
	{
		g_log->log(info, "Signal receive num:" + std::to_string(signo));
		g_log->log(info, "Server shutdown");
		delete g_log;
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

	g_log = NULL;
	g_fd = 0;
	g_sock = 0;
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
	g_fd = fd;
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
		dir = opendir(PATH_LOG);
		try
		{
			if (!dir)
			{
				if (errno == ENOENT)
				{
					if (mkdir(PATH_LOG, 0444) < 0)
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
			log = new Tintin_reporter(PATH_FILE_LOG);
			log->log(info, "Started");
		}
		catch (std::exception const& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
			unlock_deamon(&fd);
			return (EXIT_FAILURE);
		}
		g_log = log;
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		if ((sock = create_server(log)) < 0)
		{
			unlock_deamon(&fd);
			delete log;
			return (EXIT_FAILURE);
		}
		g_sock = sock;
		log->log(info, "Server created, PID: " + std::to_string(getpid()));
		run_server(&sock, log);
		log->log(info, "Server shutdown");
		unlock_deamon(&fd);
		delete log;
	}
	return (EXIT_SUCCESS);
}
