
#include "../include/ft_ping.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void print_reply(const struct s_ip_icmp *recv_packet, double ping_time);
static void print_ttl(const struct s_ip_icmp *recv_packet);
static void print_verbose(const struct s_ip_icmp *recv_packet);
static void print_echo();

static void dump_packet(const struct s_ip_icmp *packet);

void ft_ping_print(t_ping *ping, const struct s_ip_icmp *recv_packet, double ping_time)
{
	const struct icmphdr *icmp_hdr = &recv_packet->icmp.hdr;
	switch (icmp_hdr->type) {
	case ICMP_ECHO:
		print_echo();
		break;
	case ICMP_ECHOREPLY:
		if (ping->opt.flood)
			write(1, "\b \b", 3);
		else
			print_reply(recv_packet, ping_time);
		break;
	case ICMP_TIME_EXCEEDED:
		print_ttl(recv_packet);
		break;
	case ICMP_DEST_UNREACH:
		dprintf(2, "ICMP_DEST_UNREACH\n");
		break;
	default:
		printf("unkown\n");
		dump_packet(recv_packet);
	}
	if (!(icmp_hdr->type == ICMP_ECHO || icmp_hdr->type == ICMP_ECHOREPLY) && ping->opt.verbose)
		print_verbose(recv_packet);
}

static void print_reply(const struct s_ip_icmp *recv_packet, double ping_time)
{
	const struct iphdr *ip_hdr = &recv_packet->ip;
	const struct icmphdr *icmp_hdr = &recv_packet->icmp.hdr;
	const unsigned short len = ntohs(ip_hdr->tot_len) - sizeof(struct iphdr);

	char ntop_buffer[50];

	dprintf(1, "%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", len,
			inet_ntop(AF_INET, &ip_hdr->saddr, ntop_buffer, sizeof(ntop_buffer)),
			(int)htons(icmp_hdr->un.echo.sequence), (int)ip_hdr->ttl, ping_time);
}

static void print_echo()
{
	// dprintf(2, "echo\n");
}

static void print_ttl(const struct s_ip_icmp *recv_packet)
{
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

static void print_verbose(const struct s_ip_icmp *recv_packet)
{
	const struct s_ip_icmp *request_packet = (void *)&recv_packet->icmp.buffer;
	dump_packet(request_packet);
}

static void dump_packet(const struct s_ip_icmp *packet)
{
	const struct iphdr *ip = &packet->ip;
	dprintf(2, "IP Hdr Dump:\n");
	for (size_t i = 0; i < (sizeof(struct iphdr)) / 2; i++) {
		dprintf(2, " %.4x", ntohs(((short *)ip)[i]) & 0XFFFF);
	}
	dprintf(2, "\n");

	dprintf(2, "Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	Dst	Data\n");
	int version = ip->version;
	int ihl = ip->ihl;
	int tos = ip->tos;
	int len = ntohs(ip->tot_len);
	int id = ntohs(ip->id);
	int frag_off = ntohs(ip->frag_off);
	int frag = (frag_off >> 1) & 0b11;
	int off = frag_off >> 3;
	int ttl = ip->ttl;
	int pro = ip->protocol;
	int cks = ntohs(ip->check);
	char src[16] = {0};
	char dst[16] = {0};
	inet_ntop(AF_INET, &ip->saddr, src, sizeof(src));
	inet_ntop(AF_INET, &ip->daddr, dst, sizeof(dst));
	dprintf(2, "%2d %2d %3.2d %4.4x %4.4x %3d %4.4d %3.2d %3.2x %4.4x %s  %s\n", version, ihl, tos,
			len, id, frag, off, ttl, pro, cks, src, dst);

	const struct icmphdr *icmp = &packet->icmp.hdr;
	dprintf(1, "ICMP: type %d, ", icmp->type);
	dprintf(1, "code %d, ", icmp->code);
	dprintf(1, "size %d, ", len - (int)sizeof(struct iphdr));
	dprintf(1, "id 0x%x, ", icmp->un.echo.id);
	dprintf(1, "seq 0x%x", icmp->un.echo.sequence);
	dprintf(1, "\n");
}
