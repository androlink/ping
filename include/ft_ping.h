/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 15:59:35 by gcros             #+#    #+#             */
/*   Updated: 2026/04/26 18:38:52 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
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
	OT_LINGER,
	// param type int
	OT_TOS,
	// param type short
	OT_TTL,
	// param type char*
	OT_ADDR,
	// param type int
	OT_VERBOSE,
	// param type short
	OT_SEQUENCE,
	// param type short
	OT_SIZE,
	// param type int
	OT_PRELOAD,
};

struct s_stddev
{
	double agregator_si;
	double agregator_sq;
};

struct	s_ping
{
	int					sckt_fd_4;
	uint16_t			id;

	struct {
		int					tx;
		int					rx;
		double				min;
		double				max;
		double				avg;
		struct s_stddev		stddev;
		uint16_t			sequence;
	}	stat;

	struct {
		struct sockaddr_in	dest_addr;
		int					audible;
		int					flood;
		int					verbose;
		uint8_t				ttl;
		uint8_t				tos;
		uint16_t    		packet_size;
		long				count;
		int					preload;
		double				interval;
		double				linger;
	}	opt;
};

struct s_icmp_packet
{
	struct	icmphdr hdr;
	char 	buffer[PKT_MAX_SIZE - sizeof(struct	icmphdr)];
};

struct s_ip_icmp
{
	struct iphdr			ip;
	struct s_icmp_packet	icmp;
};

unsigned short			icmp_checksum(void *addr, int pckt_byte_count);
struct s_icmp_packet	init_icmp_packet(int seq, int size);
int						check_icmp_checksum(struct s_icmp_packet *pckt, int size);
int                     is_icmp(struct s_ip_icmp *recv_packet);

void			tv_print(struct timeval *tv);
struct timeval	tv_norm(struct timeval *tv);
struct timeval	tv_sub(struct timeval *tv1, struct timeval *tv2);
struct timeval	tv_add(struct timeval *tv1, struct timeval *tv2);
struct timeval	tv_from_double(double val);
double 			tv_to_double(struct timeval *tv);
double			getftime();

struct s_stddev	stddef_append(struct s_stddev aggregator, double time);
double			avg_append(double avg_agregator, int count, double num);


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
int			reset_stats(t_ping *ping);

int			ft_ping(t_ping *ping);
int			ft_ping_poll(t_ping *ping);
int			ft_ping_select(t_ping *ping);
void        ft_ping_print(t_ping *ping, const struct s_ip_icmp *recv_packet, double ping_time);


void	ft_memdump(void *p, size_t len);

#endif
