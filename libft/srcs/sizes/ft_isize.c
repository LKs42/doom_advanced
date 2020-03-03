/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppetitea <ppetitea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/27 18:03:26 by lbenard           #+#    #+#             */
/*   Updated: 2019/11/21 03:23:29 by ppetitea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sizes/isize.h"

t_isize	ft_isize(size_t x, size_t y)
{
	t_isize	retval;

	retval.x = x;
	retval.y = y;
	return (retval);
}
