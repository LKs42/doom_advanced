#include "SDL.h"
#include <stdio.h>
#include "libft.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitmap/bitmap.h"
#include "error/error.h"

//#define RATIO 16 / 9
#define HEIGHT 1024
#define WIDTH 1024
#define MAPEXPOSANT 1
//#define WIDTH (HEIGHT * RATIO)
#include <math.h>

typedef struct	s_point
{
	int x;
	int y;
}		t_point;

float lerp(float v0, float v1, float t) {
  return (1 - t) * v0 + t * v1;
}

uint32_t	pp_get_SDLcolor(SDL_Color color)
{
	uint32_t c;
	c = 0xFFFFFFFF;
	c = c | color.a;
	c = c << 8;
	c = c | color.r;
	c = c << 8;
	c = c | color.g;
	c = c << 8;
	c = c | color.b;
	return (c);
}

int pp_putint(int *pixel, int x, int y, int color)
{
	if(y < 0 || y > HEIGHT)
		return (0);
	if (x < 0 || x > WIDTH)
		return (0);
	pixel[y * WIDTH + x] = color;
	return (0);
}

void	pp_liner_int(int *pixel, t_point *a, t_point *b, int color)
{
	int tab[10];

	tab[0] = a->x;
	tab[1] = b->x;
	tab[2] = a->y;
	tab[3] = b->y;
	tab[4] = abs(tab[1] - tab[0]);
	tab[5] = tab[0] < tab[1] ? 1 : -1;
	tab[6] = abs(tab[3] - tab[2]);
	tab[7] = tab[2] < tab[3] ? 1 : -1;
	tab[8] = (tab[4] > tab[6] ? tab[4] : -tab[6]) / 2;
	while (!(tab[0] == tab[1] && tab[2] == tab[3]))
	{
		pp_putint(pixel, tab[0], tab[2], color);
		tab[9] = tab[8];
		tab[8] -= (tab[9] > -tab[4]) ? tab[6] : 0;
		tab[0] += (tab[9] > -tab[4]) ? tab[5] : 0;
		tab[8] += (tab[9] < tab[6]) ? tab[4] : 0;
		tab[2] += (tab[9] < tab[6]) ? tab[7] : 0;
	}
}

int pp_putpixel(uint32_t *pixel, int x, int y, uint32_t color)
{
	if(y < 0 || y > HEIGHT)
		return (0);
	if (x < 0 || x > WIDTH)
		return (0);
	pixel[y * WIDTH + x] = color;
	return (0);
}

void	pp_liner(uint32_t *pixel, t_point *a, t_point *b, uint32_t color)
{
	int tab[10];

	tab[0] = a->x;
	tab[1] = b->x;
	tab[2] = a->y;
	tab[3] = b->y;
	tab[4] = abs(tab[1] - tab[0]);
	tab[5] = tab[0] < tab[1] ? 1 : -1;
	tab[6] = abs(tab[3] - tab[2]);
	tab[7] = tab[2] < tab[3] ? 1 : -1;
	tab[8] = (tab[4] > tab[6] ? tab[4] : -tab[6]) / 2;
	while (!(tab[0] == tab[1] && tab[2] == tab[3]))
	{
		pp_putpixel(pixel, tab[0], tab[2], color);
		tab[9] = tab[8];
		tab[8] -= (tab[9] > -tab[4]) ? tab[6] : 0;
		tab[0] += (tab[9] > -tab[4]) ? tab[5] : 0;
		tab[8] += (tab[9] < tab[6]) ? tab[4] : 0;
		tab[2] += (tab[9] < tab[6]) ? tab[7] : 0;
	}
}

int	get_blue(uint32_t color)
{
	int c;
	color = color << 24;
	color = color >> 24;
	c = (int) color;
	return (c);
}

void	draw_vertical_line(uint32_t *pixels, int x, int ytop, int ybot, uint32_t color)
{
	if (ytop > ybot)
		return ;
	if (ytop < 0)
		ytop = 0;
	if (ybot > HEIGHT)
		ybot = HEIGHT;
	if (ybot < 0)
		ybot = 0;
	while (ytop < ybot)
		pixels[(ytop++) * WIDTH + x] = color;
}

uint32_t nocturne(uint32_t c)
{
	return((c & 255) << 8);
}

unsigned char get_r(uint32_t c)
{
	return(c & 0xFF);
}


unsigned char get_g(uint32_t c)
{
	return((c >> 8) & 0xFF);
}

unsigned char get_b(uint32_t c)
{
	return((c >> 16) & 0xFF);
}

void	draw_bg(uint32_t *pixels, uint32_t *texture)
{
	for (int i = 0; i < HEIGHT * HEIGHT; i++)
		pixels[i] = texture[i];
}

void	draw_line(int *hm, int x, int y, int size, int h, int radius)
{
	int hauteur;
	for(int i = 0; i < size * 2; i++)
	{
		if (hm[y * WIDTH + (x + i)] > h)
			hm[y * WIDTH + (x + i)] = h;
	}
}

void	bomb(int *hm, int x, int y, float radius, int h)
{
	int x1;
	int y1;
	//pour set la hm a la bonne valeur on aura
	//if(hm[x,y] > hm[xp,yp])
	//hm[x,y] = hm[xp,xyp]
	float tmp = radius;
	float inc = 0;
	float a = 0;
	h = hm[y*WIDTH*MAPEXPOSANT+x];
	ft_putnbr(h);
	while(h <= radius * 2 && a <= M_PI/2)
	{
		for(float theta = M_PI/2; theta <= (3 * M_PI)/2; theta += 0.01)
		{
			x1 = x + cos(theta) * radius;
			y1 = y + sin(theta) * radius;
			draw_line(hm, x1, y1, fabs(cos(theta)) * radius , h, radius);
		}
		h -= 1;
		//h += radius / (M_PI/2);
		inc = (tmp * cos(a));
		//a += 0.06;
		a += (M_PI/2) / (radius - h);
		//a += radius / (M_PI/2);
		radius = inc;
	}
}

void	render(uint32_t *pixels, int *hm, uint32_t *colormap, t_point player, float phi, int height, int horizon, int scale_height, int distance, uint32_t *bg)
{
	memset(pixels, 0xFFFFFFFF, sizeof(uint32_t) * WIDTH * HEIGHT);
	draw_bg(pixels, bg);
	int mapwidthperiod = MAPEXPOSANT * WIDTH - 1;
	int mapheightperiod = MAPEXPOSANT * HEIGHT - 1;

	(void)scale_height;

	float sinang = sin(phi);
	float cosang = cos(phi);

	uint32_t hiddeny[WIDTH];

	for (int i = 0; i < WIDTH; i++)
		hiddeny[i] = 0;

	for(int i = 0; i < WIDTH; i++)
		hiddeny[i] = HEIGHT;

	float deltaz = 1;

	for(float z=1; z < distance; z += deltaz)
	{
		float plx =  -cosang * z - sinang * z;
		float ply =   sinang * z - cosang * z;
		float prx =   cosang * z - sinang * z;
		float pry =  -sinang * z - cosang * z;

		float dx = (prx - plx) / WIDTH;
		float dy = (pry - ply) / WIDTH;
		plx += player.x;
		ply += player.y;
		float invz = 1 / z * 240 * scale_height;
		int mapoffset = 0;
		for(int i=0; i< WIDTH; i++)
		{
			mapoffset = (((int)floorf(ply) & (int)mapwidthperiod) << 10) + (((int)floorf(plx)) & ((int)mapheightperiod));
			float heightonscreen = (height - hm[mapoffset]) * invz + horizon;
			draw_vertical_line(pixels, i, heightonscreen, hiddeny[i], colormap[mapoffset]);
			if (heightonscreen < hiddeny[i]) hiddeny[i] = heightonscreen;
			plx += dx;
			ply += dy;
		}
		deltaz += 0.005;
	}
}

static const char *arrow[] = {
	/* width height num_colors chars_per_pixel */
	"    32    32        3            1",
	/* colors */
	"X c #00ff00",
	". c #000000",
	"  c None",
	/* pixels */
	"X                               ",
	"X.X                             ",
	"X..X                            ",
	"X...X                           ",
	"X....X                          ",
	"X.....X                         ",
	"X......X                        ",
	"X.......X                       ",
	"X........X                      ",
	"X.........X                     ",
	"X....XXXXXXX                    ",
	"X..XX                           ",
	"X.X                             ",
	"XX                              ",
	"X                               ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"0,0"
};

static SDL_Cursor *init_system_cursor(const char *image[])
{
	int i, row, col;
	Uint8 data[4*32];
	Uint8 mask[4*32];
	int hot_x, hot_y;

	i = -1;
	for (row=0; row<32; ++row) {
		for (col=0; col<32; ++col) {
			if (col % 8) {
				data[i] <<= 1;
				mask[i] <<= 1;
			} else {
				++i;
				data[i] = mask[i] = 0;
			}
			switch (image[4+row][col]) {
				case 'X':
					data[i] |= 0x01;
					mask[i] |= 0x01;
					break;
				case '.':
					mask[i] |= 0x01;
					break;
				case ' ':
					break;
			}
		}
	}
	sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
	return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

int	collision_height(int *hm, t_point *player, int *height, int playerheight)
{
	int x;
	int y;

	x = abs(player->x % WIDTH);
	y = abs(player->y % HEIGHT);
	if (*height < hm[y * WIDTH + x] + playerheight)
		*height = hm[y * WIDTH + x] + playerheight;
	return (1);
}

int	collision_walls(int *hm, t_point *player, int dx, int dy, int height, int playerheight, int wallheight)
{
	int x;
	int y;

	x = abs(player->x % WIDTH);
	y = abs(player->y % HEIGHT);
	if (hm[(y + dy) * WIDTH + (x + dx)] + playerheight > hm[y * WIDTH + x] + height + playerheight + wallheight)
		return (0);
	return (1);
}

int	collisions(int *hm, t_point *player, int *height, int playerheight, int dx, int dy, int wallheight)
{
//	if (collision_walls(hm, player, dx, dy, *height, playerheight, wallheight) == 0)
//		return (0);
	if (collision_height(hm, player, height, playerheight) == 0)
		return (0);
	return (1);
}

int main(int argc, char **argv)
{
	SDL_Event e;
	int quit = 0;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	int statut = EXIT_FAILURE;
	SDL_Texture *texture = NULL;

	t_bitmap_texture *img = load_bmp("assets/maps/volcano/heightmap.bmp");
	t_bitmap_texture *texturemap = load_bmp("assets/maps/volcano/colormap.bmp");
	t_bitmap_texture *bg = load_bmp("assets/sky/sky.bmp");
	uint32_t *map = img->pixels;
	uint32_t *colormap = texturemap->pixels;

	if(0 != SDL_Init(SDL_INIT_VIDEO))
		goto Quit;
	window = SDL_CreateWindow("DOOM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if(NULL == window)
		goto Quit;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if(NULL == renderer)
		goto Quit;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);

	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetCursor(init_system_cursor(arrow));
	SDL_RenderClear(renderer);

	uint32_t *screen;
	screen = malloc(sizeof(uint32_t) * HEIGHT * WIDTH );
	//memset(screen, 255, WIDTH * HEIGHT * (sizeof(uint32_t)));

	int *hm;
	hm = malloc(sizeof(int) * HEIGHT * WIDTH);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);

	memset(screen, 0x00000000, WIDTH * HEIGHT * sizeof(uint32_t));
	memset(hm, 0, WIDTH * HEIGHT * sizeof(int));

	t_point player;
	player.x = WIDTH/2;
	player.y = HEIGHT/2;

	(void)player.x;

	char *mapinside;
	mapinside = malloc(sizeof(char) * HEIGHT * WIDTH);
	for(int i = 0; i < WIDTH * HEIGHT; i++)
		mapinside[i] = 0;
//	fill_map(mapinside);

	for(int i = 0; i < WIDTH * HEIGHT - 1; i++)
		hm[i] = get_blue(map[i]);
	float ag = 0;
	int height = 50;
	int horizon = 650;

	float incx = 0;
	int incy = 0;
	int angley = HEIGHT / 2;
	int cursor = 0;
	int speed = 10;
	int dy = 1;
	while(!quit)
	{
		if(!cursor)
		SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
		SDL_UpdateTexture(texture, NULL, screen, WIDTH * sizeof(uint32_t));
		while(SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT) {quit = 1;}
			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_SPACE)
					height += 5;
				if (e.key.keysym.sym == SDLK_x)
					height -= 5;
				if (e.key.keysym.sym == SDLK_r)
					horizon += 5;
				if (e.key.keysym.sym == SDLK_r)
					angley += 5;
				if (e.key.keysym.sym == SDLK_f)
					angley -= 5;
				if (e.key.keysym.sym == SDLK_f)
					horizon -= 5;
				if (e.key.keysym.sym == SDLK_w)
				{
					player.y -= speed * (float)cos(ag);//10 == player speed;
					player.x -= speed * (float)sin(ag);
				}
				if (e.key.keysym.sym == SDLK_s)
				{
					player.y += speed * (float)cos(ag);//10 == player speed;
					player.x += speed * (float)sin(ag);
				}
				if (e.key.keysym.sym == SDLK_a)
				{
					player.y -= speed * (float)cos(ag + M_PI/2);//10 == player speed;
					player.x -= speed * (float)sin(ag + M_PI/2);
				}
				if (e.key.keysym.sym == SDLK_d)
				{
					player.y += speed * (float)cos(ag + M_PI/2);//10 == player speed;
					player.x += speed * (float)sin(ag + M_PI/2);
				}
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_LSHIFT)
					speed = 20;
				if (e.key.keysym.sym == SDLK_LCTRL)
				{
					cursor = cursor ? 0:1;
					cursor ? SDL_ShowCursor(SDL_ENABLE) : SDL_ShowCursor(SDL_DISABLE);
				}
				if (e.key.keysym.sym == SDLK_p)
					bomb(hm, abs(player.x % (WIDTH * MAPEXPOSANT)), abs(player.y % (HEIGHT * MAPEXPOSANT)), 50, height);
			}
			if (e.type == SDL_KEYUP)
			{
				if (e.key.keysym.sym == SDLK_LSHIFT)
					speed = 10;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (e.button.button == SDL_BUTTON_LEFT)
					printf("height:%d\n", height);
				if (e.button.button == SDL_BUTTON_RIGHT)
					printf("x:%d y: %d\n", player.x, player.y);
			}
			if (e.type == SDL_MOUSEMOTION)
			{
				//////////rc inside//////////////
				/*
				if (e.button.x > WIDTH / 2)
					ag += 0.01 * (e.button.x - WIDTH / 2);
				if (e.button.x < WIDTH / 2)
					ag -= 0.01 * (WIDTH/2 - e.button.x );

				if (e.button.y > HEIGHT / 2)
					angley -= 1 * (e.button.y - HEIGHT/2);
					//angley -= 1;
				if (e.button.y < HEIGHT / 2)
					angley += 1 * (HEIGHT/2 - e.button.y);

				*/
				/////////////////////////////////
				if (e.button.x > WIDTH / 2)
					ag -= 0.01 * (e.button.x - WIDTH / 2);
				if (e.button.x < WIDTH / 2)
					ag += 0.01 * (WIDTH/2 - e.button.x );
				if (e.button.y > HEIGHT / 2)
					horizon -= 1 * (e.button.y - HEIGHT / 2);
					//horizon -= 10;
				if (e.button.y < HEIGHT / 2)
					horizon += 1 * (HEIGHT/2 - e.button.y);
					//horizon += 10;
				/////////////////////////////////
			}
		}
		collision_height(hm, &player, &height, 10);
		render(screen, hm, colormap, player, ag, height, horizon, 2, 3000, bg->pixels);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	statut = EXIT_SUCCESS;

Quit:
	if(NULL != renderer)
		SDL_DestroyRenderer(renderer);
	if(NULL != texture)
		SDL_DestroyTexture(texture);
	if(NULL != window)
		SDL_DestroyWindow(window);
	SDL_Quit();
	return statut;
}
