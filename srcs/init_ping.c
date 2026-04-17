/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 12:55:20 by gcros             #+#    #+#             */
/*   Updated: 2026/04/17 19:37:37 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

int	init_ping(t_ping *ping)
{
	int				sckt = 0;

	*ping = (t_ping){0};
	ping->sckt_fd = -1;
	if (geteuid() == 0)
	{
		if ((sckt = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
		{
			perror("socket");
			goto on_error;
		}
	}
	else
	{
		dprintf(2, "you need to be root\n");
		goto on_error;
	}

	*ping = (t_ping){
		.rx = 0,
		.tx = 0,
		.sckt_fd = sckt,
		.id = getpid(),
		.dest_addr = (typeof(ping->dest_addr)){
			.sin_addr.s_addr = 0,
			.sin_family = AF_INET,
		},
		.flood = 0,
		.interval = 1,
		.timeout = 1,
		.ttl = 64,
		.count = MAX_COUNT,
		.audible = 0,
		.timestamp = 0,
		.sequence = 1,
		.packet_size = 64,
		.verbose = 0,
	};
	return (1);
	on_error:
	{
		if (sckt >= 0)
			close(sckt);
	}
	return -1;
}

void free_ping(t_ping *ping)
{
	if (ping->sckt_fd >= 0)
		close(ping->sckt_fd);
	ping->sckt_fd = -1;
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
		OPT_CASE(OT_SEQUENCE, ping->sequence);
		OPT_CASE(OT_SIZE, ping->packet_size);
	default:
		break;
	}
	return 0;
}
