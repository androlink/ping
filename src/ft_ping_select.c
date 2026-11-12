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

static ssize_t ping_send(t_ping *ping, struct sockaddr_in *addr, t_ping_opt *opt);
static ssize_t ping_recv(t_ping *ping, struct s_ip_icmp *recv_buf);
static void update_stats(t_ping_stat *stats, struct s_ip_icmp *recv_packet, double start);

volatile int stop = 0;

void sighandler(int _)
{
	(void)_;
	stop = 1;
}

int ft_ping_select(t_ping *ping, struct sockaddr_in *target, t_ping_opt *opt, t_ping_stat *stats)
{
	int select_ret;
	fd_set rfd;
	uint8_t recv_buffer[0xFFFF];
	// struct s_ip_icmp recv_packet;
	struct timeval select_timeout;
	double last_send = 0.;
	// struct timeval interval;

	signal(SIGINT, sighandler);
	if (opt->ttl != 0)
		setsockopt(ping->sckt_fd_4, SOL_IP, IP_TTL, &opt->ttl, sizeof(opt->ttl));
	if (opt->tos != 0)
		setsockopt(ping->sckt_fd_4, SOL_IP, IP_TOS, &opt->tos, sizeof(opt->tos));
	if (opt->flood)
		opt->interval = 0.01;

	// interval = tv_from_double(opt->interval);

	for (long preload_count = 0; preload_count < opt->preload; preload_count++) {
		// if (ping_send(ping) == -1) goto error;
		ping_send(ping, target, opt);
	}

	if (ping_send(ping, target, opt) == -1)
		goto error;

	last_send = getftime();
	select_timeout = tv_from_double(opt->linger);
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
			if (ping_send(ping, target, opt) == -1)
				goto error;
			stats->tx++;
			last_send = getftime();
			select_timeout = tv_from_double(opt->linger);
			continue;
		}
		else {
			int recv_ret = 0;
			recv_ret = ping_recv(ping, (void *)recv_buffer);
			if (recv_ret == -1)
				goto error;
			else if (recv_ret != 0) {
				double ping_time = (getftime() - last_send) * 1000.;
				update_stats(stats, (void *)recv_buffer, last_send);
				ft_ping_print(opt, (void *)recv_buffer, ping_time);
				double spend_time = getftime() - last_send;
				double rest = opt->interval - spend_time;
				if (rest < 0)
					rest = 0;
				select_timeout = tv_from_double(rest);
			}
		}
		if (stats->tx >= opt->count)
			break;
	}

	signal(SIGINT, SIG_DFL);
	return (1);
error:
	signal(SIGINT, SIG_DFL);
	return (-1);
}

static ssize_t ping_send(t_ping *ping, struct sockaddr_in *addr, t_ping_opt *opt)
{
	struct s_icmp_packet icmp_req;

	icmp_req = init_icmp_packet(ping->sequence, opt->packet_size);
	ssize_t ret = sendto(ping->sckt_fd_4, &icmp_req, sizeof(struct icmphdr) + opt->packet_size, 0,
						 (struct sockaddr *)addr, sizeof(*addr));
	if (ret == -1)
		perror("ping: sendto");
	if (opt->flood)
		write(1, ".", 1);
	if (opt->audible)
		write(1, "\a", 1);
	ping->sequence++;
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

	char ntop_buffer[50];

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
		inet_ntop(AF_INET, &recv_buf->ip.saddr, ntop_buffer, sizeof(ntop_buffer));
		dprintf(1, "checksum mismatch from %s\n", ntop_buffer);
		return 0;
	}
	if (!check_seq_id(&recv_buf->icmp.hdr, ping)) {
		return 0;
	}
	return (ret);
}

#include <math.h>

static void update_stats(t_ping_stat *stats, struct s_ip_icmp *recv_packet, double ping_time)
{
	const struct icmphdr *icmp_hdr = &recv_packet->icmp.hdr;

	if (icmp_hdr->type == ICMP_ECHOREPLY) {
		stats->avg = avg_append(stats->avg, stats->rx, ping_time);
		stats->min = stats->min == 0. ? ping_time : fmin(stats->min, ping_time);
		stats->max = stats->max == 0. ? ping_time : fmax(stats->max, ping_time);
		stats->stddev = stddef_append(stats->stddev, ping_time);
		stats->rx++;
	}
}
