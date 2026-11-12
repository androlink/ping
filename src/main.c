

#include "../include/ft_ping.h"
#include <netinet/in.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

// slow ping 5.149.253.24

static int ping_addr(t_ping *ping, char *target, t_ping_opt *opt);

void close(int);

int main(int ac, char **av)
{
	t_ping ping = {0};
	t_ping_opt option = {0};
	int opt_addr = 0;
	long ret;

	if (init_ping(&ping) == -1)
		goto on_error;
	if ((opt_addr = init_option(ac, av, &option)) == -1)
		goto on_error;

	for (int i = opt_addr; i < ac; i++) {
		ret = ping_addr(&ping, av[i], &option);
		if (ret == -1)
			goto on_error;
	}
	if (ret == 0)
		goto on_error;
	free_ping(&ping);
	return (0);
on_error: {
	free_ping(&ping);
}
	return (-1);
}

static int ping_addr(t_ping *ping, char *target, t_ping_opt *opt)
{
	char dest_ip[50];
	struct sockaddr_in dest_addr = {.sin_family = AF_INET};
	t_ping_stat stats = {0};

	if (address_resolution(target, &dest_addr) == -1)

		dprintf(1, "FT_PING %s (%s): %d data bytes", target,
				inet_ntop(AF_INET, &dest_addr.sin_addr, dest_ip, sizeof(dest_ip)),
				opt->packet_size);
	if (opt->verbose)
		dprintf(1, ", id 0x%x = %d", ping->id, ping->id);
	dprintf(1, "\n");
	int ret = ft_ping_select(ping, &dest_addr, opt, &stats);
	if (ret == -1)
		return -1;
	print_stat(stats, target);

	return 1;
}
