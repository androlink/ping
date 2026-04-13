/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 12:55:20 by gcros             #+#    #+#             */
/*   Updated: 2026/04/13 17:12:31 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

int	init_ping(char *str_dest, t_ping *ping)
{
	struct in_addr	dest_ip = {0};
	int				sckt = 0;

	*ping = (t_ping){0};
	ping->sckt_fd = -1;
	if (0 == inet_pton(AF_INET, str_dest, &dest_ip))
	{
		dprintf(2, "inet_pton: bad formating\n");
		goto on_error;
	}
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
		.target_addr = dest_ip.s_addr,
		.sckt_fd = sckt,
		.dest_addr = {
			.sin_addr = dest_ip,
			.sin_family = AF_INET
			},
	};
	return 1;
	on_error:
	{
		if (sckt >= 0)
			close(sckt);
	}
	return 0;
}

void free_ping(t_ping *ping)
{
	if (ping->sckt_fd >= 0)
		close(ping->sckt_fd);
	ping->sckt_fd = -1;
}
