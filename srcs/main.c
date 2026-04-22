/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 15:59:19 by gcros             #+#    #+#             */
/*   Updated: 2026/04/22 20:24:32 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <stdio.h>

#include <arpa/inet.h>

// slow ping 5.149.253.24

int	ping_addr(t_ping *ping, char *addr);

int	main(int ac, char **av)
{

	(void) ac;
	(void) av;
	t_ping	ping;
	int		opt_addr = 0;
	long 	ret;

	if (init_ping(&ping) == -1)	goto on_error;
	if ((opt_addr = init_option(ac, av, &ping)) == -1)	goto on_error;

	for (int i = opt_addr; i < ac; i++)
	{
		reset_stats(&ping);
		ret = ping_addr(&ping, av[i]);
		if (ret == -1)
			goto on_error;
	}
	if (ret == 0) goto on_error;
	free_ping(&ping);
	return (0);
	on_error:
	{
		free_ping(&ping);
	}
	return (-1);
}

typeof(ft_ping) *ft_ping_ptr = ft_ping_poll;

int	ping_addr(t_ping *ping, char *addr)
{
	char dest_ip[50];

	if (set_option(ping, OT_ADDR, &addr) == -1)
		return (-1);
	dprintf(1, "PING %s (%s): %d data bytes\n",
		addr,
		inet_ntop(AF_INET, &ping->opt.dest_addr.sin_addr, dest_ip, sizeof(dest_ip)),
		ping->opt.packet_size
	);
	int ret = ft_ping_ptr(ping);
	if (ret == -1)	return -1;
	dprintf(1, "--- %s ping statistics ---\n", addr);
	dprintf(1, "%d packets transmitted, %d packet reeived, %3.2f%% packet loss\n",
		ping->stat.tx, ping->stat.rx,
		(1. - (ping->stat.rx / (double)ping->stat.tx)) * 100);
	//drintf(1, "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n");

	return 1;
}