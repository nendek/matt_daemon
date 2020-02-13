#include "matt_daemon.hpp"

void		sigsig(void)
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

void		sig_handler(int signo)
{
	(void)signo;
	std::cout << "SIG HANDLER" << std::endl;
	exit(EXIT_SUCCESS);
}

