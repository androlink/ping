/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 15:59:35 by gcros             #+#    #+#             */
/*   Updated: 2026/04/14 17:22:27 by gcros            ###   ########.fr       */
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

#define PKT_SIZE 64

struct s_option
{
	int		flood:1;
	int		ttl;
	// packet timeout in second
	double	timeout;
	// send interval in second
	double	interval;
	long	count;
};

struct	s_ping
{
	int					sckt_fd;
	int					tx;
	int					rx;
	unsigned int		target_addr;
	struct sockaddr_in	dest_addr;
	t_option			option;
};

struct s_icmp_packet
{
	struct	icmphdr hdr;
	char 	buffer[PKT_SIZE - sizeof(struct	icmphdr)];
};

struct s_icmp_recv
{
	struct iphdr			ip;
	struct s_icmp_packet	icmp;
};

unsigned short	icmp_checksum(
	unsigned short *addr,
	unsigned int addr_byte_count);
struct s_icmp_packet init_icmp_packet(int seq);


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

int			init_ping(char *str_dest, t_ping *ping);
void		free_ping(t_ping *ping);

int			ft_ping(t_ping *ping, t_option *option);

#endif
