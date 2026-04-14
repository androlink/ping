/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 15:59:19 by gcros             #+#    #+#             */
/*   Updated: 2026/04/14 20:28:22 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <getopt.h>
#include <stdio.h>

// slow ping 5.149.253.24

static int	init_option(int ac, char **av, t_option *option);

int	main(int ac, char **av)
{

	(void) ac;
	(void) av;
	t_option	option;
	t_ping	ping;
	double	time_start, time_stop;

	if (ac != 2){return 1;}
	if (init_option(ac, av, &option) == 0)	goto on_error;
	if (init_ping(av[1], &ping) == 0)	goto on_error;
	time_start = getftime();
	ft_ping(&ping, &option);
	time_stop = getftime();
	dprintf(2, "tx: %d, rx: %d, total time: %.2f ms, %.3f%% packet loss\n",
		ping.tx,
		ping.rx,
		time_stop - time_start,
		(1. - (ping.rx / (double)ping.tx)) * 100);
	free_ping(&ping);
	return (0);
	on_error:
	{
		perror("");
	}
	return (1);
}

static int	init_option(int ac, char **av, t_option *option)
{
	*option = (t_option){
		.flood = 0,
		.interval = .2,
		.timeout = .1,
		.ttl = 64,
		.count = 100,
	};

	(void) ac, (void) av;
	return 1;
}
