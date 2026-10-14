

#include "../include/ft_ping.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void print_option(void);

static const struct option long_option[] = {
	{"audible", no_argument, 0, 'a'},		{"count", required_argument, 0, 'c'},
	{"flood", no_argument, 0, 'f'},			{"interval", required_argument, 0, 'i'},
	{"size", required_argument, 0, 's'},	{"ttl", required_argument, 0, 'L'},
	{"tos", required_argument, 0, 'T'},		{"linger", required_argument, 0, 'W'},
	{"verbose", no_argument, 0, 'v'},		{"help", required_argument, 0, 'h'},
	{"preload", required_argument, 0, 'l'}, {0},
};

int init_option(int ac, char **av, t_ping *ping)
{
	_Bool flood = 0;
	_Bool interval = 0;
	int ch;

	while ((ch = getopt_long(ac, av, "ac:fi:l:s:L:T:vW:h?", long_option, NULL)) != -1) {
		if (ch == '?' || ch == 'h') {
			print_option();
			return (-1);
		}
		else if (ch == 'a') {
			int i = 1;
			set_option(ping, OT_AUDIBLE, &i);
		}
		else if (ch == 'c') {
			long c = strtol(optarg, NULL, 10);
			set_option(ping, OT_COUNT, &c);
		}
		else if (ch == 'f') {
			int f = 1;
			set_option(ping, OT_FLOOD, &f);
			double i = 0.001;
			set_option(ping, OT_INTERVAL, &i);
			flood = 1;
		}
		else if (ch == 'i') {
			double i = strtod(optarg, NULL);
			if (i > 0.)
				set_option(ping, OT_INTERVAL, &i);
			interval = 1;
		}
		else if (ch == 'L') {
			uint8_t t = strtol(optarg, NULL, 10);
			set_option(ping, OT_TTL, &t);
		}
		else if (ch == 'T') {
			char t = 1;
			set_option(ping, OT_TOS, &t);
		}
		else if (ch == 'v') {
			int v = 1;
			set_option(ping, OT_VERBOSE, &v);
		}
		else if (ch == 'W') {
			double w = strtod(optarg, NULL);
			if (w > 0.)
				set_option(ping, OT_LINGER, &w);
		}
		else if (ch == 's') {
			unsigned short s = strtol(optarg, NULL, 10);
			set_option(ping, OT_SIZE, &s);
		}
		else if (ch == 'l') {
			int s = strtol(optarg, NULL, 10);
			if (s > 0)
				set_option(ping, OT_PRELOAD, &s);
		}
	}
	if (interval == 1 && flood == 1) {
		dprintf(2, "ft_ping: -f and -i incompatible options\n");
		return -1;
	}

	return optind;
}

static void print_option(void)
{
	dprintf(2, "tkt y a les options d'affiché\n");
	dprintf(2, "truc de plus de details, fait un ping -? stp\n");
}
