#include "matt_daemon.hpp"

#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include <cstring>

#define SOCKET_ERROR -1

static int	read_from_client(int fd)
{
	char	buffer[MAXMSG];
	int	nbytes;

	memset(buffer, '\0', MAXMSG);
	nbytes = read(fd, buffer, MAXMSG);
	if (nbytes < 0)
		exit (EXIT_FAILURE);
	else if (nbytes == 0)
		return (-1);
	else
	{
		std::cout << "msg=: " << buffer << std::endl;
		return (0);
	}
	return (0);
}

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
	struct sockaddr_in	info_client;
	fd_set			readfds;
	fd_set			active_fd;
	socklen_t		size;
	int			new_client;
	int			nb_client;

	nb_client = 0;
	size = sizeof(info_client);
	FD_ZERO(&active_fd);
	FD_SET(sock, &active_fd);
	while (1)
	{
		readfds = active_fd;
		if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0)
			goto error;
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &readfds))
			{
				if (i == sock)
				{
					if (nb_client < 3)
					{
						if ((new_client = accept(sock, (struct sockaddr*)&info_client, &size)) < 0)
							goto error;
						FD_SET(new_client, &active_fd);
						nb_client++;
					}
				}
				else
				{
					if (read_from_client(i) < 0)
					{
						close(i);
						FD_CLR(i, &active_fd);
						nb_client--;
					}
				}
			}
		}
	}
	return (0);
error:
	close(sock);
	return (-1);
}
