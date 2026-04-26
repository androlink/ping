/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 17:27:10 by gcros             #+#    #+#             */
/*   Updated: 2026/04/26 18:37:58 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

struct s_stddev stddef_append(struct s_stddev stddev, double time)
{
	return ((struct s_stddev){
		.agregator_sq = stddev.agregator_sq + time * time,
		.agregator_si = stddev.agregator_si + time,
		}
	);
}

double	avg_append(double avg_agregator, int count, double time)
{
	return ((avg_agregator * (count) + time) / (double)(count + 1.));
}
