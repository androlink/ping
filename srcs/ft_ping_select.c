/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping_select.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 18:27:36 by gcros             #+#    #+#             */
/*   Updated: 2026/04/24 17:25:30 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <sys/select.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <signal.h>

static ssize_t ping_send(t_ping *ping);
static ssize_t ping_recv(t_ping *ping, struct s_ip_icmp *recv_buf);
static int process_recv(t_ping *ping, struct s_ip_icmp *recv_packet, struct timeval *start);

int	stop;

void	sighandler(int)
{
	stop = 1;
}

int ft_ping_select(t_ping *ping)
{
	int ret;
	fd_set rfd;
	struct s_ip_icmp	recv_packet;
	struct timeval select_timeout, start;
	//struct timeval interval;
	stop = 0;

	signal(SIGINT, sighandler);

	//interval = tv_from_double(ping->opt.interval);

	for (long preload_count = 0; preload_count < ping->opt.preload; preload_count++)
	{
		ping_send(ping);
	}

	ping_send(ping);
	gettimeofday(&start, NULL);
	while (!stop)
	{
		select_timeout = tv_from_double(ping->opt.timeout);
		FD_ZERO(&rfd);
		FD_SET(ping->sckt_fd_4, &rfd);
		//gettimeofday(NULL, NULL);
		tv_print(&select_timeout);
		ret = select(ping->sckt_fd_4 + 1, &rfd, 0, 0, &select_timeout);
		if (ret == -1)
		{
			if (errno == EINTR) // is signal
				continue ;
			perror("ping: select");
			goto error;
		}else if (ret > 0)
		{
			int ret = 0;
			ret = ping_recv(ping, &recv_packet);
			if (ret == -1)
				goto error;
			else if (ret != 0)
			{
				ret = process_recv(ping, &recv_packet, &start);
				if (ret == 0)
					continue;
			}
		}
		else
		{
			// timeout
			dprintf(2, "timeout\n");
			if (ping_send(ping) == -1)
				goto error;
			gettimeofday(&start, NULL);
		}
		if (ping->stat.tx >= ping->opt.count)
			break ;
	}

	return (1);
	error:
	signal(SIGINT, SIG_DFL);
	return (-1);
}

static ssize_t ping_send(t_ping *ping)
{
	struct s_icmp_packet	icmp_req;

	icmp_req = init_icmp_packet(ping->stat.sequence, ping->opt.packet_size);
	ssize_t ret = sendto(ping->sckt_fd_4,
		&icmp_req,
		sizeof(struct icmphdr) + ping->opt.packet_size,
		0,
		(struct sockaddr *)&ping->opt.dest_addr,
		sizeof(ping->opt.dest_addr)
		);
	if (ret == -1)
		perror("ping: sendto");
	ping->stat.sequence++;
	ping->stat.tx++;
	return (ret);
}

static ssize_t ping_recv(t_ping *ping, struct s_ip_icmp *recv_buf)
{

	ssize_t ret = recvfrom(ping->sckt_fd_4,
		recv_buf,
		sizeof(*recv_buf),
		0,
		(struct sockaddr *)NULL,
		NULL);
	if (ret == -1)
		perror("ping: recvfrom");
	return (ret);
}

#include <math.h>

static int process_recv(t_ping *ping, struct s_ip_icmp *recv_packet, struct timeval *start)
{
	const struct iphdr		*ip_hdr = &recv_packet->ip; 
	const struct icmphdr	*icmp_hdr = &recv_packet->icmp.hdr;
	char ntop_buffer[50];

	double ping_time = (getftime() - tv_to_double(start)) * 1000.;
	switch (icmp_hdr->type)
	{
		case ICMP_ECHOREPLY:
			dprintf(2, "%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
			ping->opt.packet_size,
			inet_ntop(AF_INET, &ping->opt.dest_addr.sin_addr, ntop_buffer, sizeof(ntop_buffer)),
			(int)htons(icmp_hdr->un.echo.sequence),
			(int)ip_hdr->ttl, ping_time);
			ping->stat.rx++;
			ping->stat.avg = (ping->stat.avg * (ping->stat.rx - 1) + ping_time) / ping->stat.rx;
			ping->stat.min = ping->stat.min == 0. ? ping_time : fmin(ping->stat.min, ping_time);
			ping->stat.max = fmax(ping->stat.min, ping_time);
			(void) ping->stat.stddev;
			return 1;
		break;
		case ICMP_ECHO:
			dprintf(2, "not for me\n");
			return 0;
		case ICMP_TIME_EXCEEDED:
			dprintf(2, "reach ttl\n");
			return 1;


	}

	return (1);
}
