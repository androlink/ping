/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 12:55:20 by gcros             #+#    #+#             */
/*   Updated: 2026/04/15 15:02:03 by gcros            ###   ########.fr       */
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
	};
	return 1;
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
