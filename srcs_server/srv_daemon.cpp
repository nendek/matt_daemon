#include "matt_daemon.hpp"

#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <cstring>

#define SOCKET_ERROR -1

typedef int SOCKET;

static unsigned long	hash_djb2(unsigned char *str)
{
	unsigned long	hash = 5381;
	int		c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (hash);
}

static void		decrypt_msg(char *str, const int len)
{
	int i = 0;

	while (i < len - 1)
	{
		str[i] ^= KEY;
		i++;
	}
}

static void		encrypt_msg(char *str, const int len)
{
	int i = 0;

	while (i < len - 1)
	{
		str[i] ^= KEY;
		i++;
	}
}

static int		write_client(const SOCKET sock, char *buffer, const int len, Tintin_reporter *log)
{
	encrypt_msg(buffer, len);
	if (send(sock, buffer, len, 0) < 0)
	{
		log->log(error, strerror(errno));
		return (EXIT_FAILURE);
	}
	return (0);
}

static void		ask_passwd(const SOCKET sock, Tintin_reporter *log)
{
	char	str[] = "passwd:\n";

	write_client(sock, str, 8, log);
}

static int		read_client(const SOCKET sd, Tintin_reporter *log, uint8_t *auth)
{
	char			buffer[MAXMSG];
	int			n;
	std::string		ret;
	std::string 		str;

	memset(buffer, '\0', MAXMSG);
	n = recv(sd, buffer, MAXMSG - 1, 0);
	decrypt_msg(buffer, n);
	if (n < 0)
		return (1);
	else if (n == 0)
		return (-1);
	else
	{
		if (*auth == 0)
		{
			if (PASSWD_HASH == hash_djb2((unsigned char*)buffer))
			{
				log->log(info, "User logged");
				*auth = 1;
				return (0);
			}
			ask_passwd(sd, log);
			return (0);
		}
		str = buffer;
		ret += "User input: ";
		ret += str;
		log->log(msg, ret);
		if (str.compare("quit\n") == 0)
			return (1);
		else if (str.compare("quit") == 0)
			return (1);
		return (0);
	}
	return (0);
}

int			create_server(Tintin_reporter *log)
{
	SOCKET			sock;
	struct sockaddr_in	sin;

	sock = 0;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
	{
		log->log(error, strerror(errno));
		return (EXIT_FAILURE);
	}
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SRV_PORT);
	if ((bind(sock, (struct sockaddr*)&sin, sizeof(sin))) != SOCKET_ERROR)
		if ((listen(sock, 5)) != SOCKET_ERROR)
			return sock;
	close(sock);
	log->log(error, strerror(errno));
	return (EXIT_FAILURE);
}

int			run_server(const SOCKET *sock, Tintin_reporter *log)
{
	struct sockaddr_in	info_client;
	fd_set			readfds;
	fd_set			writefds;
	fd_set			active_fd;
	socklen_t		size;
	int			new_client;
	int			nb_client;
	int			ret;
	uint8_t			auth[3] = {0};

	nb_client = 0;
	size = sizeof(info_client);
	FD_ZERO(&active_fd);
	FD_SET(*sock, &active_fd);
	while (1)
	{
		readfds = active_fd;
		writefds = active_fd;
		if (select(FD_SETSIZE, &readfds, &writefds, NULL, NULL) < 0)
			goto err;
		(void)writefds;
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &readfds))
			{
				if (i == *sock)
				{
					if ((new_client = accept(*sock, (struct sockaddr*)&info_client, &size)) < 0)
						goto err;
					if (nb_client < 3)
					{
						FD_SET(new_client, &active_fd);
						nb_client++;
						log->log(info, "Client connected");
						ask_passwd(new_client, log);
					}
					else
					{
						close(new_client);
						log->log(info, "Client disconnected because already 3 clients connected");
					}
				}
				else
				{
					ret = read_client(i, log, &auth[i]);
					if (ret < 0)
					{
						close(i);
						FD_CLR(i, &active_fd);
						nb_client--;
						log->log(info, "Client disconnected");
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
err:
	close(*sock);
	log->log(error, strerror(errno));
	return (-1);
}
