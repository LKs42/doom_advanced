/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rgb_to_int.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbenard <lbenard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/27 16:05:22 by lbenard           #+#    #+#             */
/*   Updated: 2019/02/20 20:01:39 by lbenard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "colors/rgb.h"

t_u32	rgb_to_int(t_rgb color)
{
	int	result;

	result = color.r;
	result <<= 8;
	result += color.g;
	result <<= 8;
	result += color.b;
	return (result);
}
