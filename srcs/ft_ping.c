/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 16:33:18 by gcros             #+#    #+#             */
/*   Updated: 2026/04/15 18:46:17 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/poll.h>

static int	ping_routine(t_ping *ping, int seq, int *poll_state, double	*time_start);
static int	ping_send(t_ping *ping, struct s_icmp_packet *pckt);
static int	ping_recv(t_ping *ping, struct s_icmp_recv *pckt);
static int	process_reply(t_ping *ping, struct s_icmp_recv *recv_pckt, double time);

double getftime()
{
	struct timeval	tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000. + tv.tv_usec / 1000.);
}

int	ft_ping(t_ping *ping)
{
	int				poll_state = POLLOUT;
	int				loopcount = 1;
	double			time_start;
	int				ret;
	double			time_diff = 0;
	struct pollfd	pfd;


	{
		unsigned char time_to_live = 64;
		setsockopt(ping->sckt_fd, SOL_IP, IP_TTL, &time_to_live, sizeof(time_to_live));
	}
	pfd = (typeof(pfd)){.fd = ping->sckt_fd};
	while (loopcount <= ping->count)
	{
		pfd.events = poll_state;
		int timeout = (ping->timeout * 1000.) + 100;
		timeout = timeout - time_diff;
		if ((ret = poll(&pfd, 1, timeout)) == -1)
		{
			perror("poll");
			return (-1);
		}
		else if (ret == 0)		// on timeout
		{
			write(1, "\b|", 2);
			if (poll_state == POLLIN)
				poll_state = POLLOUT;
			else if (poll_state == POLLOUT)
				dprintf(2, "can't write on socket\n");
		}
		else
		{
			if ((ret = ping_routine(ping, loopcount, &poll_state, &time_start)) == -1)
				return (1);
			else if (ret == 0)	// if packet not for me
			{
				time_diff = (getftime() - time_start);
				continue;
			}
			else if (ret == 2)	//if packet send
			{
				time_diff = 0;
				continue;
			}
		}
		usleep(ping->interval * 1000000);
		time_diff = 0;
		loopcount++;
	}
	return 1;
}

static int	ping_routine(t_ping *ping, int seq, int *poll_state, double	*time_start)
{
	struct s_icmp_packet	pckt;
	struct s_icmp_recv		recv_pkt;
	double					time_stop;

	if (*poll_state == POLLOUT){
		pckt = init_icmp_packet(seq);
		*time_start = getftime();
		ping->tx++;
		if (ping_send(ping, &pckt) == -1)
			return (-1);
		if (ping->audible) write(2, "\a", 1);
		write(1, ".", 1);
		*poll_state = POLLIN;
		return (2);
	}
	else if (*poll_state == POLLIN){
		recv_pkt = (typeof(recv_pkt)){0};
		time_stop = getftime();
		if (ping_recv(ping, &recv_pkt) == -1)
			return (-1);
		if (recv_pkt.icmp.hdr.un.echo.sequence != seq)
		{
			return (0);
		}
		write(1, "\b", 2);
		*poll_state = POLLOUT;
		ping->rx++;
		process_reply(ping, &recv_pkt, time_stop - *time_start);
	}
	return (1);
}

static int	process_reply(t_ping *ping, struct s_icmp_recv *recv_pckt, double time)
{
	const struct iphdr		*ip_hdr = &recv_pckt->ip; 
	const struct icmphdr	*icmp_hdr = &recv_pckt->icmp.hdr;

	(void) ping;
	if (icmp_hdr->type == ICMP_TIME_EXCEEDED)
	{
		printf("%d bytes from %s: icmp_seq=%d Time to live exceeded\n",
			PKT_SIZE, "todo", (int)icmp_hdr->un.echo.sequence);
	}
	else if (icmp_hdr->type == ICMP_ECHOREPLY)
	{
		char dest_ip[50];
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
			PKT_SIZE, inet_ntop(AF_INET, &ping->dest_addr.sin_addr, dest_ip, sizeof(dest_ip)), (int)icmp_hdr->un.echo.sequence,
			(int)ip_hdr->ttl, time);
	}
	else
	{
		
	}

	return 1;
}

static int	ping_send(t_ping *ping, struct s_icmp_packet *pckt)
{
	ssize_t len = sendto(ping->sckt_fd, pckt, sizeof(*pckt), 0,
			(struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr));

	if (len <= 0)
	{
		perror("sendto");
		return -1;
	}

	return 1;
}

static int	ping_recv(t_ping *ping, struct s_icmp_recv *pckt)
{
	struct sockaddr_in	*target;
	socklen_t 			addr_len = sizeof(target);

	ssize_t len = recvfrom(ping->sckt_fd, pckt, sizeof(*pckt), 0,
			(struct sockaddr *)&target, &addr_len);

	if (len <= 0)
	{
		perror("recvfrom");
		return -1;
	}
	return 1;
}
