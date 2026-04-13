/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcros <gcros@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 15:17:04 by gcros             #+#    #+#             */
/*   Updated: 2026/04/11 15:39:35 by gcros            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

int	internal_error = 0;

const char	*str_ierror(int code)
{
	static char *const	error_list[] = {
	[IE_NOP] = "no error",
	[IE_BAD_ARG] = "Bad parameters",
	[IE_NO_ARG] = "missing parameters",
	[IE_SOCKET] = "Socket",
	[IE_BAD_PERM] = "you don't have the permission",
	[IE_ADDRESS_FORMAT] = "invalid address",
	};

	return (error_list[code]);
}

int	need_errno(int code)
{
	static int	list[] = {
	[IE_NOP] = 0,
	[IE_BAD_ARG] = 0,
	[IE_SOCKET] = 1,
	[IE_BAD_PERM] = 0,
	[IE_ADDRESS_FORMAT] = 0,
	};

	return (list[code]);
}

void	print_error(int code)
{
	if (need_errno(code))
		printf("%s: %s\n", str_ierror(code), strerror(errno));
	else
		printf("%s\n", str_ierror(code));
}
