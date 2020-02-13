#include "matt_daemon.hpp"

int		check_credentials(void)
{
	uid_t cred;

	if ((cred = geteuid()) == 0)
		return (EXIT_SUCCESS);
	std::cout << "Error: not root credentials" << std::endl;
	return (EXIT_FAILURE);
}
