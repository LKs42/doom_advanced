/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_bmp.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppetitea <ppetitea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/12 15:36:31 by ppetitea          #+#    #+#             */
/*   Updated: 2020/01/12 23:12:56 by ppetitea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "bitmap/bitmap.h"
#include "error/error.h"
#include <stdlib.h>
#include <stdio.h>

static t_result			fill_pixels(FILE *file, uint32_t *pixels,
						int32_t width, int32_t height)
{
	t_argb			color;
	int32_t			x;
	int32_t			y;

	if (file == NULL || pixels == NULL)
		return (throw_error("fill_pixels", "NULL pointer provided"));
	y = 0;
	while (y < height)
	{
		x = 0;
		while (x < width)
		{
			fread(&color, 1, 3, file);
			pixels[x + (height - y - 1) * width] = color.px;
			x++;
		}
		fseek(file, (x * 3) % 4, SEEK_CUR);
		y++;
	}
	return (OK);
}

t_bitmap_texture	*load_bmp(char *path)
{
	FILE				*file;
	t_bitmap_texture	*bmp;

	if (path == NULL)
		return (throw_null("load_bmp", "NULL pointer provided"));
	if (!(bmp = (t_bitmap_texture*)malloc(sizeof(t_bitmap_texture))))
		return (throw_null("load_bmp", "bmp texture -> malloc failed"));
	if (!(file = fopen(path, "rb")))
		return (throw_null("load_bmp", "wrong path provided"));
	fread(&bmp->head, sizeof(t_bitmap_header), 1, file);
	fseek(file, bmp->head.offset, SEEK_SET);
	if (bmp->head.type != 0x4d42)
		return (throw_null("load_bmp", "file provided is not a bmp"));
	if (!(bmp->pixels = (uint32_t*)malloc(sizeof(uint32_t)
		* bmp->head.width * bmp->head.height)))
		return (throw_null("load_bmp", "pixels array -> malloc failed"));
	if (!(fill_pixels(file, bmp->pixels, bmp->head.width, bmp->head.height)))
		return (throw_null("load_bmp", "bmp_to_pixel_array failed"));
	return (bmp);
}