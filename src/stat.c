

#include "../include/ft_ping.h"
#include <math.h>
#include <stdio.h>

struct s_stddev stddef_append(struct s_stddev stddev, double time)
{
	return ((struct s_stddev){
		.agregator_sq = stddev.agregator_sq + time * time,
		.agregator_si = stddev.agregator_si + time,
	});
}

double avg_append(double avg_agregator, int count, double time)
{
	return ((avg_agregator * (count) + time) / (double)(count + 1.));
}

void print_stat(t_ping_stat stat, char *addr)
{

	double stddev = stat.stddev.agregator_sq - 2. * stat.avg * stat.stddev.agregator_si +
					stat.avg * stat.avg * (double)stat.rx;

	stddev = sqrt(stddev / (double)stat.rx);
	dprintf(1, "--- %s ping statistics ---\n", addr);
	dprintf(1, "%d packets transmitted, %d packet received, %3.2f%% packet loss\n", stat.tx,
			stat.rx, (1. - (stat.rx / (double)stat.tx)) * 100);
	if (stat.rx != 0)
		dprintf(1, "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", stat.min, stat.avg,
				stat.max, stddev);
}
