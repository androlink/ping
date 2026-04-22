/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping_select.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 18:27:36 by gcros             #+#    #+#             */
/*   Updated: 2026/04/22 20:22:46 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <sys/select.h>
#include <errno.h>
#include <stdio.h>

static int send_routine(t_ping *ping);
static int recv_routine(t_ping *ping);
static int process_recv(t_ping *ping);


int ft_ping_select(t_ping *ping)
{
	long count = 0;
	int ret;
	fd_set rfd;
	struct timeval select_timeout;


	for (long preload_count = 0;preload_count < ping->opt.preload; preload_count++)
	{
		send_routine(ping);
	}

	
	while (count < ping->opt.count)
	{
		send_routine(ping);
		FD_ZERO(&rfd);
		FD_SET(ping->sckt_fd_4, &rfd);
		do
		{
			ret = select(ping->sckt_fd_4 + 1, &rfd, 0, 0, 0);
			if (ret == -1)
			{
				if (errno == EINTR) // is signal
					continue ;
				perror("ping: select");
				return (-1);
			}else if (ret > 0)
			{
				recv_routine(ping);
			}
			else
			{
				// timeout
				break ;
			}
		}while (0); // bad packet
	}
	return (1);
}

static int send_routine(t_ping *ping)
{

	return (1);
}

static int recv_routine(t_ping *ping)
{
	return (1);
}

static int process_recv(t_ping *ping)
{
	return (1);
}
