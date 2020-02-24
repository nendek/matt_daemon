#include "matt_daemon.hpp"

#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <cstring>

#define SOCKET_ERROR -1

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
		return (-1);
	}
	return (0);
}

static void		send_shutdown(client_t *client, const int len, Tintin_reporter *log)
{
	char	str[] = "shutdown\0";

	for (int i = 0; i < len; i++)
	{
		if (client[i].sock != 0)
			write_client(client[i].sock, str, 8, log);
	}
}

static int		all_close(client_t *client, const int len)
{
	for (int i = 0; i < len; i++)
	{
		if (client[i].sock != 0)
			return (0);
	}
	return (1);
}

static void		ask_passwd(const SOCKET sock, Tintin_reporter *log)
{
	char	str[] = "passwd:\n\0";

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

static void		init_client(client_t *client, int len)
{
	for (int i = 0; i < len; i++)
	{
		client[i].auth = 0;
		client[i].sock = 0;
	}
}

static void		clear_client(client_t *client, const int i)
{
	if (client[i].sock > 0)
		close(client[i].sock);
	client[i].sock = 0;
	client[i].auth = 0;
}

static void		clear_clients(client_t *client, const int len)
{
	for (int i = 0; i < len; i++)
		clear_client(client, i);
}

static int		get_max(client_t *client, const int len, const SOCKET srv)
{
	int max = srv;

	for (int i = 0; i < len; i++)
	{
		if (client[i].sock > max)
			max = client[i].sock;
	}
	return (max);
}

static void		add_sock_client(client_t *client, const int len, const SOCKET sock)
{
	for (int i = 0; i < len; i++)
	{
		if (client[i].sock == 0)
		{
			client[i].sock = sock;
			return;
		}
	}
}

static int		get_client_by_sock(client_t *client, const int len, const SOCKET sock)
{
	for (int i = 0; i < len; i++)
	{
		if (client[i].sock == sock)
			return (i);
	}
	return (0);
}

int			run_server(const SOCKET *sock, Tintin_reporter *log)
{
	struct sockaddr_in	info_client;
	fd_set			readfds;
	fd_set			active_fd;
	socklen_t		size;
	int			new_client;
	int			nb_client;
	int			in_close;
	int			ret;
	int			max;
	client_t		client[MAXCLIENT];

	nb_client = 0;
	in_close = 0;
	init_client(client, MAXCLIENT);
	size = sizeof(info_client);
	FD_ZERO(&active_fd);
	FD_SET(*sock, &active_fd);
	while (1)
	{
		max = get_max(client, nb_client, *sock);
		readfds = active_fd;
		if (select(max + 1, &readfds, NULL, NULL, NULL) < 0)
			goto err;
		for (int i = 0; i < max + 1; i++)
		{
			if (FD_ISSET(i, &readfds))
			{
				if (i == *sock)
				{
					if ((new_client = accept(*sock, (struct sockaddr*)&info_client, &size)) < 0)
						goto err;
					if (nb_client < MAXCLIENT)
					{
						FD_SET(new_client, &active_fd);
						if (max < new_client)
							max = new_client;
						nb_client++;
						log->log(info, "Client connected");
						add_sock_client(client, MAXCLIENT, new_client);
						ask_passwd(new_client, log);
					}
					else
					{
						close(new_client);
						log->log(info, "Client disconnected because already max clients connected");
					}
				}
				else
				{
					int index_client = get_client_by_sock(client, MAXCLIENT, i);
					ret = read_client(i, log, &(client[index_client]).auth);
					if (ret < 0)
					{
						FD_CLR(i, &active_fd);
						nb_client--;
						clear_client(client, index_client);
						log->log(info, "Client disconnected");
					}
					else if (ret == 1)
					{
						in_close = 1;
						log->log(info, "Server in shutdown process");
						send_shutdown(client, MAXCLIENT, log);
						log->log(info, "Server send shutdown all clients");
					}
				}
			}
		}
		if (in_close == 1 && (all_close(client, MAXCLIENT)) == 1)
		{
			clear_clients(client, MAXCLIENT);
			close(*sock);
			return (0);
		}
	}
	return (0);
err:
	clear_clients(client, MAXCLIENT);
	close(*sock);
	log->log(error, strerror(errno));
	return (-1);
}

int			create_server(Tintin_reporter *log)
{
	SOCKET			sock;
	struct sockaddr_in	sin;

	sock = 0;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
	{
		log->log(error, strerror(errno));
		return (-1);
	}
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SRV_PORT);
	if ((bind(sock, (struct sockaddr*)&sin, sizeof(sin))) != SOCKET_ERROR)
		if ((listen(sock, 5)) != SOCKET_ERROR)
			return sock;
	close(sock);
	log->log(error, strerror(errno));
	return (-1);
}

