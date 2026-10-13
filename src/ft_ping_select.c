/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping_select.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 18:27:36 by gcros             #+#    #+#             */
/*   Updated: 2026/05/01 20:16:48 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_ping.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

static ssize_t ping_send(t_ping *ping);
static ssize_t ping_recv(t_ping *ping, struct s_ip_icmp *recv_buf);
static int process_recv(t_ping *ping, struct s_ip_icmp *recv_packet, double start);

volatile int stop = 0;

void sighandler(int _)
{
	(void)_;
	stop = 1;
}

int ft_ping_select(t_ping *ping)
{
	int select_ret;
	fd_set rfd;
	uint8_t recv_buffer[0xFFFF];
	// struct s_ip_icmp recv_packet;
	struct timeval select_timeout;
	double last_send = 0.;
	// struct timeval interval;

	signal(SIGINT, sighandler);
	if (ping->opt.ttl != 0)
		setsockopt(ping->sckt_fd_4, SOL_IP, IP_TTL, &ping->opt.ttl, sizeof(ping->opt.ttl));
	if (ping->opt.tos != 0)
		setsockopt(ping->sckt_fd_4, SOL_IP, IP_TOS, &ping->opt.tos, sizeof(ping->opt.tos));
	if (ping->opt.flood)
		ping->opt.interval = 0.01;

	// interval = tv_from_double(ping->opt.interval);

	for (long preload_count = 0; preload_count < ping->opt.preload; preload_count++) {
		// if (ping_send(ping) == -1) goto error;
		ping_send(ping);
	}

	if (ping_send(ping) == -1)
		goto error;

	last_send = getftime();
	select_timeout = tv_from_double(ping->opt.linger);
	while (!stop) {
		FD_ZERO(&rfd);
		FD_SET(ping->sckt_fd_4, &rfd);

		// tv_print(&select_timeout);
		select_ret = select(ping->sckt_fd_4 + 1, &rfd, 0, 0, &select_timeout);

		if (select_ret == -1) {
			if (errno == EINTR) // is signal
				continue;
			perror("ping: select");
			goto error;
		}
		else if (select_ret == 0) {
			// timeout
			// dprintf(2, "timeout\n");
			if (ping_send(ping) == -1)
				goto error;
			last_send = getftime();
			select_timeout = tv_from_double(ping->opt.linger);
			continue;
		}
		else {
			int recv_ret = 0;
			recv_ret = ping_recv(ping, (void *)recv_buffer);
			if (recv_ret == -1)
				goto error;
			else if (recv_ret != 0) {
				process_recv(ping, (void *)recv_buffer, last_send);
				double spend_time = getftime() - last_send;
				double rest = ping->opt.interval - spend_time;
				if (rest < 0)
					rest = 0;
				select_timeout = tv_from_double(rest);
			}
		}
		if (ping->stat.tx >= ping->opt.count)
			break;
	}

	signal(SIGINT, SIG_DFL);
	return (1);
error:
	signal(SIGINT, SIG_DFL);
	return (-1);
}

static ssize_t ping_send(t_ping *ping)
{
	struct s_icmp_packet icmp_req;

	icmp_req = init_icmp_packet(ping->sequence, ping->opt.packet_size);
	ssize_t ret = sendto(ping->sckt_fd_4, &icmp_req, sizeof(struct icmphdr) + ping->opt.packet_size,
						 0, (struct sockaddr *)&ping->opt.dest_addr, sizeof(ping->opt.dest_addr));
	if (ret == -1)
		perror("ping: sendto");
	if (ping->opt.flood)
		write(1, ".", 1);
	if (ping->opt.audible)
		write(1, "\a", 1);
	ping->sequence++;
	ping->stat.tx++;
	return (ret);
}

int check_seq_id(struct icmphdr *icmp, t_ping *ping)
{

	const uint16_t id = icmp->un.echo.id;
	const uint16_t seq = ntohs(icmp->un.echo.sequence);

	switch (icmp->type) {
	case ICMP_ECHO:
	case ICMP_ECHOREPLY:
		if (id != ping->id) {
#if DEBUG
			dprintf(2, "bad id %x\b", id);
#endif
			return 0;
		}
		if (seq + 1 != ping->sequence) {
#if DEBUG
			dprintf(2, "bad seq\b");
#endif
			return 0;
		}
		break;
	default:
		break;
	}

	return 1;
}

static ssize_t ping_recv(t_ping *ping, struct s_ip_icmp *recv_buf)
{

	ssize_t ret =
		recvfrom(ping->sckt_fd_4, recv_buf, sizeof(*recv_buf), 0, (struct sockaddr *)NULL, NULL);
	if (ret == -1) {
		perror("ping: recvfrom");
		return -1;
	}
	if ((size_t)ret < sizeof(struct iphdr)) {
		return 0;
	}
	if (!is_icmp(recv_buf)) {
		return 0;
	}
	if (!check_icmp_checksum(&recv_buf->icmp, ret)) {
		return 0;
	}
	if (!check_seq_id(&recv_buf->icmp.hdr, ping)) {
		return 0;
	}
	return (ret);
}

#include <math.h>

static int process_recv(t_ping *ping, struct s_ip_icmp *recv_packet, double start)
{
	const struct icmphdr *icmp_hdr = &recv_packet->icmp.hdr;

	double ping_time = (getftime() - start) * 1000.;
	if (icmp_hdr->type == ICMP_ECHOREPLY) {
		ping->stat.avg = avg_append(ping->stat.avg, ping->stat.rx, ping_time);
		ping->stat.min = ping->stat.min == 0. ? ping_time : fmin(ping->stat.min, ping_time);
		ping->stat.max = ping->stat.max == 0. ? ping_time : fmax(ping->stat.max, ping_time);
		ping->stat.stddev = stddef_append(ping->stat.stddev, ping_time);
		ping->stat.rx++;
	}

	ft_ping_print(ping, recv_packet, ping_time);

	return (1);
}
