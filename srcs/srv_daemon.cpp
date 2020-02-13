#include "matt_daemon.hpp"

#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define SOCKET_ERROR -1

int	create_server(void)
{
	int			sock;
	struct sockaddr_in	sin;

	sock = 0;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
		return (EXIT_FAILURE);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SRV_PORT);
	if ((bind(sock, (struct sockaddr*)&sin, sizeof(sin))) != SOCKET_ERROR)
		if ((listen(sock, 5)) != SOCKET_ERROR)
			return sock;
	close(sock);
	return (-1);
}

int	run_server(int sock)
{
	fd_setreadfds;

	while (1)
	{
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0)
			goto error;
	}

	return (0);
error:
	close(sock);
	return (-1);
}
