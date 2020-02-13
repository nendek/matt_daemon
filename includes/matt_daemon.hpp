#ifndef MATT_DEAMON_HPP
# define MATT_DEAMON_HPP

# include <iostream>

# include <unistd.h>
# include <signal.h>

# include <sys/stat.h>
# include <sys/types.h>
# include <sys/file.h>

# define SRV_PORT 42

/*------------------ lock_daemon ------------------*/
int		lock_daemon(int *fd);
void		unlock_deamon(int *fd);
/*------------------ cred_daemon ------------------*/
int		check_credentials(void);
/*------------------ sig_daemon -------------------*/
void		sigsig(void);
void		sig_handler(int signo);
/*------------------ sig_daemon -------------------*/
int		create_server(void);
int		run_server(int sock);

#endif
