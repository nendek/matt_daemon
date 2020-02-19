#include "matt_daemon.hpp"

int		check_credentials(void)
{
	uid_t cred;

	if ((cred = geteuid()) == 0)
		return (EXIT_SUCCESS);
	throw Error(1, "not root credentials", 2);
	return (EXIT_FAILURE);
}
