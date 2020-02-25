/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   throw_error.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppetitea <ppetitea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/11 23:15:40 by ppetitea          #+#    #+#             */
/*   Updated: 2020/01/11 23:26:09 by ppetitea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_result	throw_error(char *location, char *message)
{
	ft_putstr(location);
	ft_putstr("() -> ");
	ft_putstr(message);
	ft_putchar('\n');
	return (ERROR);
}
