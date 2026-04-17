/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 16:33:18 by gcros             #+#    #+#             */
/*   Updated: 2026/04/17 19:36:53 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/poll.h>

static int	ping_routine(t_ping *ping, int *poll_state, double	*time_start);
static int	pkt_send(t_ping *ping, struct s_icmp_packet *pckt);
static int	pkt_recv(t_ping *ping, struct s_icmp_recv *pckt);
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
	long			loopcount = 0;
	double			time_start;
	int				ret;
	double			time_diff = 0;
	struct pollfd	pfd;


	{
		setsockopt(ping->sckt_fd, SOL_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl));
	}
	
	pfd = (typeof(pfd)){.fd = ping->sckt_fd};
	while (loopcount < ping->count)
	{
		pfd.events = poll_state;
		int timeout = (ping->timeout * 1000.) + 100;
		timeout = timeout - time_diff;
		ret = poll(&pfd, 1, timeout);
		if (ret == -1)
		{
			if (errno == EAGAIN)
			{
				time_diff = (getftime() - time_start);
				continue ;
			}
			perror("poll");
			return (-1);
		}
		else if (ret == 0)		// on timeout
		{
			if (ping->flood) (void) !write(2, "\b|", 2);
			else dprintf(2, "packet timeout: %d\n", ping->sequence);
			if (poll_state == POLLIN)
				poll_state = POLLOUT;
			else if (poll_state == POLLOUT)
				dprintf(2, "can't write on socket\n");
		}
		else
		{
			if ((ret = ping_routine(ping, &poll_state, &time_start)) == -1)
				return (-1);
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
		ping->sequence++;
		loopcount++;
	}
	return loopcount;
}

static int	pkt_check(t_ping *ping, struct s_icmp_recv *recv_pkt)
{
	if (recv_pkt->icmp.hdr.type == ICMP_ECHO)
	{
		dprintf(2, "bad protocol type: %d\n", recv_pkt->icmp.hdr.type);
		return (0);
	}
	if (recv_pkt->icmp.hdr.un.echo.id != getpid())
	{
		dprintf(2, "bad id: %d\n", recv_pkt->icmp.hdr.un.echo.id);
		return (0);
	}
	if (recv_pkt->icmp.hdr.un.echo.sequence != ping->sequence)
	{
		dprintf(2, "bad sequence: %d\n", recv_pkt->icmp.hdr.un.echo.sequence);
		return (0);
	}
	return (1);
}

static int	ping_routine(t_ping *ping, int *poll_state, double	*time_start)
{
	struct s_icmp_packet	pckt;
	struct s_icmp_recv		recv_pkt;
	double					time_stop;

	if (*poll_state == POLLOUT){
		pckt = init_icmp_packet(ping->sequence, ping->packet_size);
		*time_start = getftime();
		ping->tx++;
		if (pkt_send(ping, &pckt) == -1)
			return (-1);
		if (ping->audible) (void) !write(2, "\a", 1);
		if (ping->flood) (void) !write(2, ".", 1);
		*poll_state = POLLIN;
		return (2);
	}
	else if (*poll_state == POLLIN){
		recv_pkt = (typeof(recv_pkt)){0};
		time_stop = getftime();
		if (pkt_recv(ping, &recv_pkt) == -1)
			return (-1);
		if (pkt_check(ping, &recv_pkt) == 0)
		{
			return (0);
		}
		*poll_state = POLLOUT;
		process_reply(ping, &recv_pkt, time_stop - *time_start);
	}
	return (1);
}

static int	process_reply(t_ping *ping, struct s_icmp_recv *recv_pckt, double time)
{
	const struct iphdr		*ip_hdr = &recv_pckt->ip; 
	const struct icmphdr	*icmp_hdr = &recv_pckt->icmp.hdr;

	
	if (icmp_hdr->type == ICMP_TIME_EXCEEDED)
	{
		printf("%d bytes from %s: icmp_seq=%d Time to live exceeded\n",
			ping->packet_size, "todo", (int)icmp_hdr->un.echo.sequence);
	}
	else if (icmp_hdr->type == ICMP_ECHOREPLY)
	{
		ping->rx++;
		char dest_ip[50];
		if (ping->flood) (void) !write(2, "\b", 2);
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
			ping->packet_size,
			inet_ntop(AF_INET, &ping->dest_addr.sin_addr, dest_ip, sizeof(dest_ip)),
			(int)icmp_hdr->un.echo.sequence,
			(int)ip_hdr->ttl, time);
	}
	else
	{
		printf("other ICMP response found: %d\n", icmp_hdr->type);
	}

	return 1;
}

static int	pkt_send(t_ping *ping, struct s_icmp_packet *pckt)
{
	ssize_t len = sendto(ping->sckt_fd, pckt, sizeof(pckt->hdr) + ping->packet_size, 0,
			(struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr));

	if (len <= 0)
	{
		perror("sendto");
		return -1;
	}

	return 1;
}

static int	pkt_recv(t_ping *ping, struct s_icmp_recv *pckt)
{
	struct sockaddr_in	*target;
	socklen_t 			addr_len = sizeof(target);

	ssize_t len = recvfrom(ping->sckt_fd, pckt,
		sizeof(*pckt), 0,
			(struct sockaddr *)&target, &addr_len);
	if (len <= 0)
	{
		perror("recvfrom");
		return -1;
	}
	pckt->icmp.hdr.un.echo.sequence = ntohs(pckt->icmp.hdr.un.echo.sequence);
	return 1;
}
