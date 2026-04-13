/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 16:33:18 by gcros             #+#    #+#             */
/*   Updated: 2026/04/13 19:52:23 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

static int	ping_send(t_ping *ping, struct s_icmp_packet *pckt);
static int	ping_recv(t_ping *ping, struct s_icmp_recv *pckt);
static int	process_reply(t_ping *ping, struct s_icmp_recv *recv_pckt, double time);

int	check_seq(struct s_icmp_packet *pckt1, struct s_icmp_packet *pckt2)
{
	return (pckt1->hdr.un.echo.sequence == pckt2->hdr.un.echo.sequence);
}

double getftime()
{
	struct timeval	tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000. + tv.tv_usec / 1000.);
}

int	ft_ping(t_ping *ping, t_option *option)
{
	struct s_icmp_packet	pckt;
	struct s_icmp_recv		recv_pkt;
	int						state;
	int						loopcount = 0;
	double					ping_start, ping_stop;

	{
		unsigned char time_to_live = 64;
		setsockopt(ping->sckt_fd, SOL_IP, IP_TTL, &time_to_live, sizeof(time_to_live));
	}
	while (loopcount < option->count)
	{
		loopcount++;
		pckt = init_icmp_packet(loopcount);
		ping_start = getftime();
		
		if (ping_send(ping, &pckt))
		{
			return (1);
		}
		state = 1;
		
		do{
			recv_pkt = (typeof(recv_pkt)){0};
			if (ping_recv(ping, &recv_pkt))
			{
				return (1);
			}
			ping_stop = getftime();
		}while (!check_seq(&recv_pkt.icmp, &pckt));
		state = 2;
		ping_stop = getftime();
		process_reply(ping, &recv_pkt, ping_stop - ping_start);

		usleep(option->interval * 1000);
	}
	return 0;
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

	return 0;
}

static int	ping_send(t_ping *ping, struct s_icmp_packet *pckt)
{
	ssize_t len = sendto(ping->sckt_fd, pckt, sizeof(*pckt), 0,
			(struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr));

	if (len <= 0)
	{
		perror("sendto");
		return 1;
	}

	return 0;
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
		return 1;
	}
	return 0;
}
