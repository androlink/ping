/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 12:55:20 by gcros             #+#    #+#             */
/*   Updated: 2026/04/20 17:54:23 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int address_resolution(t_ping *ping, char *addr);

int	init_ping(t_ping *ping)
{
	int				sckt = 0;

	*ping = (t_ping){0};
	ping->sckt_fd_4 = -1;
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
		.sckt_fd_4 = sckt,
		.id = getpid(),
		.stat = {
			.rx = 0,
			.tx = 0,

			.sequence = 1,
		},
		.opt = {
			.dest_addr = (typeof(ping->opt.dest_addr)){
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
			.packet_size = 56,
			.verbose = 0,
		}
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
	if (ping->sckt_fd_4 >= 0)
		close(ping->sckt_fd_4);
	ping->sckt_fd_4 = -1;
}

// todo dns resolution
int	set_option(t_ping *ping, enum e_option_type type, void *param)
{
	#define OPT_CASE(COND, FLD) case COND: FLD = *(typeof(FLD) *) param; break

	switch (type)
	{
		OPT_CASE(OT_FLOOD, ping->opt.flood);
		OPT_CASE(OT_COUNT, ping->opt.count);
		OPT_CASE(OT_INTERVAL, ping->opt.interval);
		OPT_CASE(OT_TIMEOUT, ping->opt.timeout);
		OPT_CASE(OT_TTL, ping->opt.ttl);
		OPT_CASE(OT_AUDIBLE, ping->opt.audible);
		OPT_CASE(OT_TIMESTAMP, ping->opt.timestamp);
		OPT_CASE(OT_VERBOSE, ping->opt.verbose);
		OPT_CASE(OT_SEQUENCE, ping->stat.sequence);
		OPT_CASE(OT_SIZE, ping->opt.packet_size);
		case OT_ADDR:
			char *addr = *(typeof(addr) *)param;
			return (address_resolution(ping, addr));
		break;
	default:
		break;
	}
	return 0;
}

#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#endif
#include <netdb.h>

int address_resolution(t_ping *ping, char *addr)
{
	struct addrinfo *result;

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_RAW
    };
	
	if (getaddrinfo(addr, NULL, &hints, &result) != 0)
	{
		dprintf(2, "Failed to resolve address '%s': %s\n",
                addr, "todo");
		return (-1);
	}
	for (const struct addrinfo *ai = result; ai != NULL; ai = ai->ai_next)
	{
		char	hostname[1025];
		char	hostaddr[1025];
		
		int resn = getnameinfo(ai->ai_addr, ai->ai_addrlen, hostaddr, 1025, NULL, 0, NI_NUMERICHOST);
		int resa = getnameinfo(ai->ai_addr, ai->ai_addrlen, hostname, 1025, NULL, 0, 0);
		if (resa != 0 || resn != 0)
			continue;
		printf("Hostname for %s: %s\n", hostaddr, hostname);
		if (inet_pton(AF_INET, hostaddr, &ping->opt.dest_addr.sin_addr) != 1)
		{
			dprintf(2, "Failed to resolve address '%s'\n",
                hostaddr);
			goto on_error;
		}
		break ;
	}
	freeaddrinfo(result);
	return (0);
	on_error:
	freeaddrinfo(result);
	return (-1);
}

int	reset_stats(t_ping *ping)
{
	ping->stat = (typeof(ping->stat)){0};
	return 1;
}
