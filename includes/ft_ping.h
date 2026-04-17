/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 15:59:35 by gcros             #+#    #+#             */
/*   Updated: 2026/04/17 18:33:11 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <sys/time.h>
#include <errno.h>

typedef struct s_ping	t_ping;
typedef struct s_option	t_option;

typedef int				t_socket;

// default timeout in second
# define DEFAULT_TIMEOUT 1.
// max ping loop count
# define MAX_COUNT ((~0UL) >> 1)

#define PING_INTERVAL 1.

#define PKT_MAX_SIZE 0x10000

enum e_option_type
{
	// param type long
	OT_COUNT,
	// param type int
	OT_FLOOD,
	// param type int
	OT_AUDIBLE,
	// param type double
	OT_INTERVAL,
	// param type double
	OT_TIMEOUT,
	// param type int
	OT_TTL,
	// param type in_addr_t
	OT_ADDR,
	// param type int
	OT_TIMESTAMP,
	// param type int
	OT_VERBOSE,
	// param type short
	OT_SEQUENCE,
	// param type int
	OT_SIZE,
};

struct	s_ping
{
	int					sckt_fd;
	int					tx;
	int					rx;
	int					id;
	// print a '\a' when send
	int					audible;
	// print a '.' when send
	int					flood;
	int					verbose;
	int					timestamp;
	// packet time to live in second
	int					ttl;
	// packet timeout in second
	double				timeout;
	// send interval in second
	double				interval;

	long				count;

	short				sequence;

	int					packet_size;

	struct sockaddr_in	dest_addr;
};

struct s_icmp_packet
{
	struct	icmphdr hdr;
	char 	buffer[PKT_MAX_SIZE - sizeof(struct	icmphdr)];
};

struct s_icmp_recv
{
	struct iphdr			ip;
	struct s_icmp_packet	icmp;
};

unsigned short	icmp_checksum(void *addr, int pckt_byte_count);
struct s_icmp_packet init_icmp_packet(int seq, int size);


int	init_option(int ac, char **av, t_ping *ping);
int	set_option(t_ping *ping, enum e_option_type type, void *param);

enum e_ierror_code
{
	IE_NOP,
	IE_BAD_ARG,
	IE_NO_ARG,
	IE_SOCKET,
	IE_BAD_PERM,
	IE_ADDRESS_FORMAT,
};

const char	*str_ierror(int code);
int			need_errno(int code);
void		print_error(int code);

double getftime();

int			init_ping(t_ping *ping);
void		free_ping(t_ping *ping);

int			ft_ping(t_ping *ping);

#endif
