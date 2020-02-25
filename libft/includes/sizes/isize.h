/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isize.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppetitea <ppetitea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/19 18:25:56 by lbenard           #+#    #+#             */
/*   Updated: 2019/11/21 03:22:47 by ppetitea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISIZE_H
# define ISIZE_H

# include <string.h>

/*
** 2D signed size
*/
typedef struct	s_isize
{
	size_t	x;
	size_t	y;
}				t_isize;

/*
** Constructors
*/
t_isize			ft_isize(size_t x, size_t y);

#endif
