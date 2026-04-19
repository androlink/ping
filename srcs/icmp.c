/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 16:11:44 by gcros             #+#    #+#             */
/*   Updated: 2026/04/19 19:11:31 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct s_icmp_packet init_icmp_packet(int seq, int size)
{
	struct s_icmp_packet pckt = {0};

	pckt = (typeof(pckt)){
		.hdr =
			{
				.type = ICMP_ECHO,
				.un.echo.id = getpid(),
				.un.echo.sequence = htons(seq),
			},
	};
	memset(&pckt.buffer, 0x42, size);
	pckt.hdr.checksum = icmp_checksum(
		(unsigned short *)&pckt,
		sizeof(struct icmphdr) + size
	);
	return pckt;
}

int	check_icmp_checksum(struct s_icmp_packet *pckt, int size)
{
	const uint16_t	old_chsm = pckt->hdr.checksum;

	pckt->hdr.checksum = 0;
	uint16_t chsm = icmp_checksum(
		(unsigned short *)pckt,
		sizeof(struct icmphdr) + size
	);
	pckt->hdr.checksum = old_chsm;
	return (old_chsm == chsm);
}

unsigned short	icmp_checksum(void *pckt, int pckt_byte_count)
{
	unsigned short	*buff = pckt;
	unsigned int	sum = 0;
	unsigned short	result = 0;

	while (pckt_byte_count > 1)
	{
		sum += *buff;
		pckt_byte_count -= 2;
		buff += 1;
	}
	if (pckt_byte_count == 1)
		sum += *((unsigned char *)buff);

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	result = ~sum;
	return (result);
}
