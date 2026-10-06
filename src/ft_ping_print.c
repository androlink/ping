
#include "../include/ft_ping.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void print_reply(struct s_ip_icmp *recv_packet, double ping_time);
void print_ttl(t_ping *ping, struct s_ip_icmp *recv_packet);
void print_echo();

void ft_ping_print(t_ping *ping, struct s_ip_icmp *recv_packet, double ping_time)
{
	const struct icmphdr *icmp_hdr = &recv_packet->icmp.hdr;
	switch (icmp_hdr->type) {
	case ICMP_ECHO:
		print_echo();
		break;
	case ICMP_ECHOREPLY:
		print_reply(recv_packet, ping_time);
		break;
	case ICMP_TIME_EXCEEDED:
		print_ttl(ping, recv_packet);
		break;
	default:
		printf("unkown\n");
	}
}

void print_reply(struct s_ip_icmp *recv_packet, double ping_time)
{
	const struct iphdr *ip_hdr = &recv_packet->ip;
	const struct icmphdr *icmp_hdr = &recv_packet->icmp.hdr;
	const unsigned short len = ntohs(ip_hdr->tot_len) - sizeof(struct iphdr);

	char ntop_buffer[50];

	dprintf(1, "%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", len,
			inet_ntop(AF_INET, &ip_hdr->daddr, ntop_buffer, sizeof(ntop_buffer)),
			(int)htons(icmp_hdr->un.echo.sequence), (int)ip_hdr->ttl, ping_time);
}

void print_echo()
{
	dprintf(2, "echo\n");
}

void print_ttl(t_ping *ping, struct s_ip_icmp *recv_packet)
{
	(void)ping;
	const struct iphdr *ip_hdr = &recv_packet->ip;
	const unsigned short len = ntohs(ip_hdr->tot_len) - sizeof(struct iphdr);

	char ntop_buffer[50];
	char hostname[255];

	struct sockaddr_in saddr = {.sin_family = AF_INET, .sin_addr = {ip_hdr->saddr}};

	int resa = getnameinfo((struct sockaddr *)&saddr, sizeof(struct sockaddr_in), hostname,
						   sizeof(hostname), NULL, 0, NI_NAMEREQD);
	if (resa == 0)
		dprintf(1, "%d bytes from %s (%s): Time to live exceeded\n", len, hostname,
				inet_ntop(AF_INET, &ip_hdr->saddr, ntop_buffer, sizeof(ntop_buffer)));
	else
		dprintf(1, "%d bytes from %s: Time to live exceeded\n", len,
				inet_ntop(AF_INET, &ip_hdr->saddr, ntop_buffer, sizeof(ntop_buffer)));
}
