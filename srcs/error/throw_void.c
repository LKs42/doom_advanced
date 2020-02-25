/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   throw_void.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppetitea <ppetitea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/11 23:15:40 by ppetitea          #+#    #+#             */
/*   Updated: 2020/01/12 00:39:23 by ppetitea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	throw_void(char *location, char *message)
{
	ft_putstr(location);
	ft_putstr("() -> ");
	ft_putstr(message);
	ft_putchar('\n');
}
