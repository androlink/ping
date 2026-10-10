

#include "../include/ft_ping.h"
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/types.h>

// slow ping 5.149.253.24

int ping_addr(t_ping *ping, char *addr);
void print_stat(t_ping *ping, char *addr);

void close(int);

int main(int ac, char **av)
{
	(void)ac;
	(void)av;
	t_ping ping;
	int opt_addr = 0;
	long ret;

	if (init_ping(&ping) == -1)
		goto on_error;
	if ((opt_addr = init_option(ac, av, &ping)) == -1)
		goto on_error;

	for (int i = opt_addr; i < ac; i++) {
		reset_stats(&ping);
		ret = ping_addr(&ping, av[i]);
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

typeof(ft_ping) *ft_ping_ptr = ft_ping_select;

int ping_addr(t_ping *ping, char *addr)
{
	char dest_ip[50];

	if (set_option(ping, OT_ADDR, &addr) == -1)
		return (-1);
	dprintf(1, "FT_PING %s (%s): %d data bytes", addr,
			inet_ntop(AF_INET, &ping->opt.dest_addr.sin_addr, dest_ip, sizeof(dest_ip)),
			ping->opt.packet_size);
	if (ping->opt.verbose)
		dprintf(1, ", id 0x%x = %d", ping->id, ping->id);
	dprintf(1, "\n");
	int ret = ft_ping_ptr(ping);
	if (ret == -1)
		return -1;
	print_stat(ping, addr);

	return 1;
}

#include <math.h>

void print_stat(t_ping *ping, char *addr)
{

	double stddev = ping->stat.stddev.agregator_sq -
					2. * ping->stat.avg * ping->stat.stddev.agregator_si +
					ping->stat.avg * ping->stat.avg * (double)ping->stat.rx;

	stddev = sqrt(stddev / (double)ping->stat.rx);
	dprintf(1, "--- %s ping statistics ---\n", addr);
	dprintf(1, "%d packets transmitted, %d packet received, %3.2f%% packet loss\n", ping->stat.tx,
			ping->stat.rx, (1. - (ping->stat.rx / (double)ping->stat.tx)) * 100);
	if (ping->stat.rx != 0)
		dprintf(1, "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", ping->stat.min,
				ping->stat.avg, ping->stat.max, stddev);
}
