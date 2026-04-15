/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checksum.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 17:05:46 by gcros             #+#    #+#             */
/*   Updated: 2026/04/15 16:16:37 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
