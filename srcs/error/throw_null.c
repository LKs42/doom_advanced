/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   throw_null.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppetitea <ppetitea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/11 23:15:40 by ppetitea          #+#    #+#             */
/*   Updated: 2020/01/11 23:26:16 by ppetitea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*throw_null(char *location, char *message)
{
	ft_putstr(location);
	ft_putstr("() -> ");
	ft_putstr(message);
	ft_putchar('\n');
	return (NULL);
}
