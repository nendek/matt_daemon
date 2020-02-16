#ifndef _MATT_DEAMON_HPP_
# define _MATT_DEAMON_HPP_

# include <iostream>

# include <unistd.h>
# include <signal.h>

# include <sys/stat.h>
# include <sys/types.h>
# include <sys/file.h>

#include "reporter.hpp"

# define SRV_PORT 4242
# define MAXMSG 128

/*------------------ lock_daemon ------------------*/
int		lock_daemon(int *fd);
void		unlock_deamon(int *fd);
/*------------------ cred_daemon ------------------*/
int		check_credentials(void);
/*------------------ sig_daemon -------------------*/
void		sigsig(void);
void		sig_handler(int signo);
/*------------------ sig_daemon -------------------*/
int		create_server(Tintin_reporter *log);
int		run_server(const int *sock, Tintin_reporter *log);

#endif
