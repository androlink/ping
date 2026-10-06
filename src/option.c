

#include "../include/ft_ping.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

static void print_option(void);

static const struct option long_option[] = {
	{"audible", no_argument, 0, 'a'},		{"count", required_argument, 0, 'c'},
	{"flood", no_argument, 0, 'f'},			{"interval", required_argument, 0, 'i'},
	{"size", required_argument, 0, 's'},	{"ttl", required_argument, 0, 't'},
	{"timestamp", no_argument, 0, 'T'},		{"timeout", required_argument, 0, 'W'},
	{"verbose", no_argument, 0, 'v'},		{"help", required_argument, 0, 'h'},
	{"preload", required_argument, 0, 'l'}, {0},
};

int init_option(int ac, char **av, t_ping *ping)
{

	int ch;
	while ((ch = getopt_long(ac, av, "ac:fi:l:s:t:T:vW:h?", long_option, NULL)) != -1) {
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
		}
		else if (ch == 'i') {
			double i = strtod(optarg, NULL);
			set_option(ping, OT_INTERVAL, &i);
		}
		else if (ch == 't') {
			int t = strtol(optarg, NULL, 10);
			set_option(ping, OT_TTL, &t);
		}
		else if (ch == 'T') {
			int t = 1;
			set_option(ping, OT_TIMESTAMP, &t);
		}
		else if (ch == 'v') {
			int v = 1;
			set_option(ping, OT_VERBOSE, &v);
		}
		else if (ch == 'W') {
			double w = strtod(optarg, NULL);
			set_option(ping, OT_TIMEOUT, &w);
		}
		else if (ch == 's') {
			int s = strtol(optarg, NULL, 10);
			set_option(ping, OT_SIZE, &s);
		}
		else if (ch == 'l') {
			int s = strtol(optarg, NULL, 10);
			set_option(ping, OT_PRELOAD, &s);
		}
	}
	return optind;
}

static void print_option(void)
{
	dprintf(2, "tkt y a les options d'affiché\n");
	dprintf(2, "truc de plus de details, fait un ping -? stp\n");
}
