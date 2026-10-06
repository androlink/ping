

#include <stdio.h>
#include <sys/time.h>

struct timeval tv_sub(struct timeval *tv1, struct timeval *tv2)
{
	return ((struct timeval){
		.tv_sec = tv1->tv_sec - tv2->tv_sec,
		.tv_usec = tv1->tv_usec - tv2->tv_usec,
	});
}

struct timeval tv_add(struct timeval *tv1, struct timeval *tv2)
{
	return ((struct timeval){
		.tv_sec = tv1->tv_sec + tv2->tv_sec,
		.tv_usec = tv1->tv_usec + tv2->tv_usec,
	});
}

struct timeval tv_norm(struct timeval *tv)
{
	struct timeval ret = *tv;
	while (ret.tv_usec < 0) {
		ret.tv_usec += 1000000;
		ret.tv_sec -= 1;
	}
	while (ret.tv_usec >= 1000000) {
		ret.tv_usec -= 1000000;
		ret.tv_sec += 1;
	}
	return ret;
}
struct timeval tv_from_double(double val)
{
	return (struct timeval){.tv_sec = val, .tv_usec = ((long)(val * 1000000.) % 1000000)};
}

double tv_to_double(struct timeval *tv)
{
	return (tv->tv_sec + tv->tv_usec / 1000000.);
}

void tv_print(struct timeval *tv)
{
	dprintf(2, "%ld.%lds\n", tv->tv_sec, tv->tv_usec);
}

double getftime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1000000.);
}
