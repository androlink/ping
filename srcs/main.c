/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 15:59:19 by gcros             #+#    #+#             */
/*   Updated: 2026/04/13 19:42:02 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <getopt.h>
#include <stdio.h>


static int	init_option(int ac, char **av, t_option *option);

int	main(int ac, char **av)
{

	(void) ac;
	(void) av;
	t_option	option;
	t_ping	ping;

	if (ac != 2){return 1;}
	if (init_option(ac, av, &option) == 0)	goto on_error;
	if (init_ping(av[1], &ping) == 0)	goto on_error;
	ft_ping(&ping, &option);
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
		.interval = PING_INTERVAL,
		.timout = DEFAULT_TIMEOUT,
		.ttl = 64,
		.count = MAX_COUNT,
	};

	(void) ac, (void) av;
	return 1;
}
