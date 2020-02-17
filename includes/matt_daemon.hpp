#ifndef _MATT_DEAMON_HPP_
# define _MATT_DEAMON_HPP_

# include <iostream>

# include <unistd.h>
# include <signal.h>
# include <dirent.h>
# include <errno.h>
# include <string.h>

# include <sys/stat.h>
# include <sys/types.h>
# include <sys/file.h>

# include "Reporter.hpp"
# include "Error_daemon.hpp"

# define SRV_PORT 4242
# define MAXMSG 128
# define PATH_LOG "/var/log/matt_daemon"
# define PATH_FILE_LOG "/var/log/matt_daemon/matt_daemon.log"
# define PATH_FILE_LOCK "/var/lock/matt_daemon.lock"

/*------------------ lock_daemon ------------------*/
int		lock_daemon(int *fd);
void		unlock_deamon(int *fd);
/*------------------ cred_daemon ------------------*/
int		check_credentials(void);
/*------------------ sig_daemon -------------------*/
int		create_server(Tintin_reporter *log);
int		run_server(const int *sock, Tintin_reporter *log);

#endif
