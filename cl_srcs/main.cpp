#include "ben_afk.hpp"

static void	encrypt_msg(char *str, int len)
{
	int i = 0;

	while (i < len - 1)
	{
		str[i] ^= KEY;
		i++;
	}
}

static int	create_client(void)
{
	int			sock = 0;
	struct sockaddr_in	sin;
	struct protoent		*proto = NULL;

	if ((proto = getprotobyname("tcp")) == NULL)
	{
		perror("getprotobyname");
		exit(errno);
	}
	if ((sock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) == -1)
	{
		perror("socket");
		exit(errno);
	}
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = inet_addr(ADDR);
	if ((connect(sock, (const struct sockaddr *)&sin, sizeof(sin))) == -1)
	{
		perror("connect");
		exit(errno);
	}
	return (sock);
}

int	main(void)
{
	int	sock;
	char	buff[BUFF_SIZE];
	int	ret;
	int	end = 0;

	sock = create_client();
	while (1)
	{
		if ((ret = read(STDIN_FILENO, buff, BUFF_SIZE)) == -1)
		{
			perror("read");
			return (errno);
		}
		buff[ret - 1] = '\0';
		if (!strcmp(buff, "quit"))
			end = 1;
		encrypt_msg(buff, ret);
		if (send(sock, buff, ret, 0) == -1)
		{
			perror("send");
			return (errno);
		}
		if (end)
			return (0);
		memset(&buff, '\0', BUFF_SIZE);

	}
	return (0);
}
