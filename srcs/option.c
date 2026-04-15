/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   option.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 14:46:57 by gcros             #+#    #+#             */
/*   Updated: 2026/04/15 18:48:27 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <getopt.h>
#include <arpa/inet.h>
#include <stdio.h>

static void	print_option(void);

static const struct option long_option[] = {
	{"audible", no_argument, 0, 'a'},
	{"count", required_argument, 0, 'c'},
	{"flood", no_argument, 0, 'f'},
	{"interval", required_argument, 0, 'i'},
	{"ttl", required_argument, 0, 't'},
	{"timestamp", no_argument, 0, 'T'},
	{"verbose", no_argument, 0, 'v'},
	{"timeout", required_argument, 0, 'W'},
	{"help", required_argument, 0, 'h'},
	{0},
	
};

int	init_option(int ac, char **av, t_ping *ping)
{
	ping->flood = 0,
	ping->interval = .002,
	ping->timeout = .001,
	ping->ttl = 64,
	ping->count = 100,
	ping->dest_addr = (typeof(ping->dest_addr)){
		.sin_addr.s_addr = 0,
		.sin_family = AF_INET
	};
	ping->audible = 0;
	ping->timestamp = 0;

	int ch;
	while ((ch = getopt_long(ac, av, "ac:fi:t:T:vW:h?", long_option, NULL)) != -1)
	{
		dprintf(2, "option found: %c with value %s\n", ch, optarg);
		if (ch == '?' || ch == 'h')
		{
			print_option();
			return (-1);
		}else if (ch == 'a'){
			int i = 1;
			set_option(ping, OT_AUDIBLE, &i);
		}else if (ch == 'c'){
		}else if (ch == 'f'){
			int i = 1;
			set_option(ping, OT_FLOOD, &i);
		}else if (ch == 'i'){
		}else if (ch == 't'){
		}else if (ch == 'T'){
			int i = 1;
			set_option(ping, OT_TIMESTAMP, &i);
		}else if (ch == 'v'){
			int i = 1;
			set_option(ping, OT_VERBOSE, &i);
		}else if (ch == 'W'){
		}
	}
	for (int i = 0; i < ac; i++)
		dprintf(2, "argv[%d] = %s\n", i, av[i]);

	char *str_addr = av[ac - 1];

	if (0 == inet_pton(AF_INET, str_addr, &ping->dest_addr.sin_addr))
	{
		dprintf(2, "inet_pton: bad formating\n");
		return (-1);
	}

	return 1;
}

int	set_option(t_ping *ping, enum e_option_type type, void *param)
{
	#define OPT_CASE(COND, FLD) case COND: FLD = *(typeof(FLD) *) param; break

	switch (type)
	{
		OPT_CASE(OT_FLOOD, ping->flood);
		OPT_CASE(OT_COUNT, ping->count);
		OPT_CASE(OT_INTERVAL, ping->interval);
		OPT_CASE(OT_TIMEOUT, ping->timeout);
		OPT_CASE(OT_TTL, ping->ttl);
		OPT_CASE(OT_ADDR, ping->dest_addr.sin_addr.s_addr);
		OPT_CASE(OT_AUDIBLE, ping->audible);
		OPT_CASE(OT_TIMESTAMP, ping->timestamp);
		OPT_CASE(OT_VERBOSE, ping->verbose);
	default:
		break;
	}
	return 0;
}

static void	print_option(void)
{
	dprintf(2, "tkt y a les options d'affiché\n");
	dprintf(2, "truc de plus de details, fait un ping -? stp\n");
}
