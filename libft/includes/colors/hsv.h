/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hsv.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbenard <lbenard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/19 18:38:50 by lbenard           #+#    #+#             */
/*   Updated: 2019/02/20 17:06:04 by lbenard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HSV_H
# define HSV_H

# include "types.h"

/*
** HSV color
*/
typedef struct	s_hsv
{
	float	h;
	float	s;
	float	v;
}				t_hsv;

/*
** Constructors
*/
t_hsv			ft_hsv(float h, float s, float v);

/*
** Casts
*/
t_u8			hsv_to_int(t_hsv color);

#endif
