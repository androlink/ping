/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 16:11:44 by gcros             #+#    #+#             */
/*   Updated: 2026/04/13 18:45:08 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <string.h>
#include <unistd.h>

struct s_icmp_packet init_icmp_packet(int seq)
{
	struct s_icmp_packet pckt = {0};
	
	pckt = (typeof(pckt)){
		.hdr = {
			.type = ICMP_ECHO,
			.un.echo.id = getpid(),
			.un.echo.sequence = seq,
			},
	};
	memset(&pckt.buffer, 0x42, sizeof(pckt.buffer));
	pckt.hdr.checksum = icmp_checksum((unsigned short *)&pckt, PKT_SIZE);
	return pckt;
}
