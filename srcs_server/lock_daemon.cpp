#include "matt_daemon.hpp"

int		lock_daemon(int *fd)
{
	*fd = open(PATH_FILE_LOCK, O_WRONLY | O_CREAT);
	if (*fd > 0)
	{
		if ((flock(*fd, LOCK_EX | LOCK_NB)) < 0)
		{
			close(*fd);
			goto err;
		}
		return (EXIT_SUCCESS);
	}
err:
	throw Error(1, "Matt_daemon already run, file locked", 2);
	return (EXIT_FAILURE);
}

void		unlock_deamon(int *fd)
{
	flock(*fd, LOCK_UN);
	close(*fd);
	*fd = 0;
	unlink(PATH_FILE_LOCK);
}
