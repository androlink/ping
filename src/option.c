

#include "../include/ft_ping.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void print_option(void);

static const struct option long_option[] = {
	{"audible", no_argument, 0, 'a'},
	{"count", required_argument, 0, 'c'},
	{"flood", no_argument, 0, 'f'},
	{"interval", required_argument, 0, 'i'},
	{"size", required_argument, 0, 's'},
	{"ttl", required_argument, 0, 'L'},
	{"tos", required_argument, 0, 'T'},
	{"linger", required_argument, 0, 'W'},
	{"verbose", no_argument, 0, 'v'},
	{"help", required_argument, 0, 'h'},
	{"preload", required_argument, 0, 'l'},
	{"quiet", no_argument, 0, 'q'},
	{0},
};

int init_option(int ac, char **av, t_ping_opt *opt)
{
	_Bool flood = 0;
	_Bool interval = 0;
	int ch;
	*opt = default_opt();

	while ((ch = getopt_long(ac, av, "ac:fi:l:s:L:qT:vW:h?", long_option, NULL)) != -1) {
		if (ch == '?' || ch == 'h') {
			print_option();
			return (-1);
		}
		else if (ch == 'a') {
			int i = 1;
			set_option(opt, OT_AUDIBLE, &i);
		}
		else if (ch == 'c') {
			long c = strtol(optarg, NULL, 10);
			set_option(opt, OT_COUNT, &c);
		}
		else if (ch == 'f') {
			int f = 1;
			set_option(opt, OT_FLOOD, &f);
			double i = 0.001;
			set_option(opt, OT_INTERVAL, &i);
			flood = 1;
		}
		else if (ch == 'i') {
			double i = strtod(optarg, NULL);
			if (i > 0.)
				set_option(opt, OT_INTERVAL, &i);
			interval = 1;
		}
		else if (ch == 'L') {
			uint8_t t = strtol(optarg, NULL, 10);
			set_option(opt, OT_TTL, &t);
		}
		else if (ch == 'T') {
			char t = 1;
			set_option(opt, OT_TOS, &t);
		}
		else if (ch == 'v') {
			int v = 1;
			set_option(opt, OT_VERBOSE, &v);
		}
		else if (ch == 'W') {
			double w = strtod(optarg, NULL);
			if (w > 0.)
				set_option(opt, OT_LINGER, &w);
		}
		else if (ch == 's') {
			unsigned short s = strtol(optarg, NULL, 10);
			set_option(opt, OT_SIZE, &s);
		}
		else if (ch == 'l') {

			int q = 1;
			set_option(opt, OT_PRELOAD, &q);
		}
		else if (ch == 'q') {
			int s = strtol(optarg, NULL, 10);
			if (s > 0)
				set_option(opt, OT_QUIET, &s);
		}
	}
	if (interval == 1 && flood == 1) {
		dprintf(2, "ft_ping: -f and -i incompatible options\n");
		return -1;
	}
	if (optind == ac) {
		dprintf(1, "missing host operand\n");
		dprintf(1, "try 'ft_ping --help' or 'ft_ping --?' for more information.\n");
		return -1;
	}
	return optind;
}

static void print_option(void)
{
	dprintf(1, "FT_PING\n");
	dprintf(1, "usage: ft_ping [OPTION...] HOST ...\n");
	dprintf(1, "\n");
	dprintf(1, "-h | ?, --help\t\tdisplay help menu\n");
	dprintf(1, "-v, --verbose\t\tverbose output\n");
	dprintf(1, "-i, --interval=NUMBER\t\twait NUMBER seconds between sending each packet\n");
	dprintf(1, "-L, --ttl=N\t\tspecify N as time-to-live\n");
	dprintf(1, "-T, --tos=N\t\tset type of service (TOS) to N\n");
	dprintf(1, "-W, --linger=N\t\tnumber of seconds to wait for response\n");
	dprintf(1, "-f, --flood\t\tflood ping\n");
	dprintf(1, "-l, --preload=NUMBER\t\tsend NUMBER packet as fast as possible before falling into "
			   "normal mode of behavior (not implemented)\n");
	dprintf(1, "-f, --flood\t\tflood ping\n");
	dprintf(1, "-q, --quiet\t\tflood ping\n");
	dprintf(1, "-s, --size=NUMBER\t\tsend NUMBER data octets\n");
	dprintf(1, "-c, --count=NUMBER\t\tstop after sending NUMBER packet\n");
}
