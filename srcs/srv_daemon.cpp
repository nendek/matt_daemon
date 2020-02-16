#include "matt_daemon.hpp"

#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include <cstring>

#define SOCKET_ERROR -1

static int	read_from_client(int fd, Tintin_reporter *log)
{
	char		buffer[MAXMSG];
	int		nbytes;
	std::string	ret;

	memset(buffer, '\0', MAXMSG);
	nbytes = read(fd, buffer, MAXMSG);
	if (nbytes < 0)
		exit (EXIT_FAILURE);
	else if (nbytes == 0)
		return (-1);
	else
	{
		std::string str;
		str = buffer;
		ret += "Receive msg: ";
		ret += str;
		log->log(msg, ret);
		if (str.compare("quit\n") == 0)
			return (1);
		if (str.compare("quit") == -1)
			return (1);
		return (0);
	}
	return (0);
}

int	create_server(Tintin_reporter *log)
{
	int			sock;
	struct sockaddr_in	sin;

	sock = 0;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
	{
		log->log(error, "create socket");
		return (EXIT_FAILURE);
	}
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SRV_PORT);
	if ((bind(sock, (struct sockaddr*)&sin, sizeof(sin))) != SOCKET_ERROR)
		if ((listen(sock, 5)) != SOCKET_ERROR)
			return sock;
	close(sock);
	log->log(error, "create socket");
	return (-1);
}

int	run_server(const int *sock, Tintin_reporter *log)
{
	struct sockaddr_in	info_client;
	fd_set			readfds;
	fd_set			active_fd;
	socklen_t		size;
	int			new_client;
	int			nb_client;
	int			ret;

	nb_client = 0;
	size = sizeof(info_client);
	FD_ZERO(&active_fd);
	FD_SET(*sock, &active_fd);
	while (1)
	{
		readfds = active_fd;
		if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0)
			goto error;
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &readfds))
			{
				if (i == *sock)
				{
					if ((new_client = accept(*sock, (struct sockaddr*)&info_client, &size)) < 0)
						goto error;
					if (nb_client < 3)
					{
						FD_SET(new_client, &active_fd);
						nb_client++;
					}
					else
						close(new_client);
				}
				else
				{
					ret = read_from_client(i, log);
					if (ret < 0)
					{
						close(i);
						FD_CLR(i, &active_fd);
						nb_client--;
					}
					else if (ret == 1)
					{
						close(*sock);
						return (0);
					}
				}
			}
		}
	}
	return (0);
error:
	close(*sock);
	log->log(error, "run server");
	return (-1);
}
