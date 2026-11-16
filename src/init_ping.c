

#include "../include/ft_ping.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int init_ping(t_ping *ping)
{
	int sckt = 0;

	*ping = (t_ping){0};
	ping->sckt_fd_4 = -1;
	if (geteuid() == 0) {
		if ((sckt = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
			perror("socket");
			goto on_error;
		}
	}
	else {
		dprintf(2, "you need to be root\n");
		goto on_error;
	}

	*ping = (t_ping){
		.sckt_fd_4 = sckt,
		.id = getpid(),
	};
	return (1);
on_error: {
	if (sckt >= 0)
		close(sckt);
}
	return -1;
}

t_ping_opt default_opt()
{
	return (t_ping_opt){
		.flood = 0,
		.interval = 1.0,
		.linger = 1.0,
		.ttl = 0,
		.tos = 0,
		.count = MAX_COUNT,
		.audible = 0,
		.packet_size = 56,
		.verbose = 0,
	};
}

void free_ping(t_ping *ping)
{
	if (ping->sckt_fd_4 >= 0)
		close(ping->sckt_fd_4);
	ping->sckt_fd_4 = -1;
}

int set_option(t_ping_opt *opt, enum e_option_type type, void *param)
{
#define OPT_CASE(COND, FLD)                                                                        \
	case COND:                                                                                     \
		FLD = *(typeof(FLD) *)param;                                                               \
		break

	switch (type) {
		OPT_CASE(OT_FLOOD, opt->flood);
		OPT_CASE(OT_COUNT, opt->count);
		OPT_CASE(OT_INTERVAL, opt->interval);
		OPT_CASE(OT_LINGER, opt->linger);
		OPT_CASE(OT_TOS, opt->tos);
		OPT_CASE(OT_TTL, opt->ttl);
		OPT_CASE(OT_AUDIBLE, opt->audible);
		OPT_CASE(OT_VERBOSE, opt->verbose);
		OPT_CASE(OT_SIZE, opt->packet_size);
		OPT_CASE(OT_PRELOAD, opt->preload);
		OPT_CASE(OT_QUIET, opt->quiet);
	default:
		break;
	}
	return 0;
}

#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#endif
#include <netdb.h>

int address_resolution(char *target, struct sockaddr_in *dest_addr)
{
	struct addrinfo *result;

	struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_RAW};

	if (getaddrinfo(target, NULL, &hints, &result) != 0) {
		dprintf(2, "Failed to resolve address '%s': %s\n", target, "todo");
		return (-1);
	}

	for (const struct addrinfo *ai = result; ai != NULL; ai = ai->ai_next) {
		char hostname[255];
		char hostaddr[INET_ADDRSTRLEN];

		int resn = getnameinfo(ai->ai_addr, ai->ai_addrlen, hostaddr, sizeof(hostaddr), NULL, 0,
							   NI_NUMERICHOST);
		int resa = getnameinfo(ai->ai_addr, ai->ai_addrlen, hostname, sizeof(hostname), NULL, 0, 0);
		if (resa != 0 || resn != 0)
			continue;

#if DEBUG
		dprintf(2, "Hostname for %s: %s\n", hostaddr, hostname);
#endif
		if (inet_pton(AF_INET, hostaddr, &dest_addr->sin_addr) != 1) {
			dprintf(2, "Failed to resolve address '%s'\n", hostaddr);
			goto on_error;
		}
		break;
	}
	freeaddrinfo(result);
	return (0);
on_error:
	freeaddrinfo(result);
	return (-1);
}
