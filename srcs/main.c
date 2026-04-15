/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 15:59:19 by gcros             #+#    #+#             */
/*   Updated: 2026/04/15 16:53:53 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <stdio.h>

// slow ping 5.149.253.24

int	main(int ac, char **av)
{

	(void) ac;
	(void) av;
	t_ping	ping;
	double	time_start, time_stop;

	if (init_ping(&ping) == -1)	goto on_error;
	if (init_option(ac, av, &ping) == -1)	goto on_error;
	time_start = getftime();
	ft_ping(&ping);
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
	}
	return (1);
}
