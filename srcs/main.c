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
	int z;
}		t_point;

typedef struct	s_vec2fl
{
	float x;
	float y;
}		t_vec2fl;

typedef struct	s_screen
{
	int width;
	int height;
	uint32_t *pixels;
}		t_screen;

typedef struct	s_map
{
	char *name;
	int width;
	int height;
	uint32_t *heightmap;
	uint32_t *colormap;
}		t_map;

typedef struct	s_player
{
	int life;
	t_point pos;
	int fov;
	int horizon;
	int view_distance;
	float speed;
	float view_direction;
}		t_player;

typedef enum e_GAME_STATE
{
	GAME,
	PAUSE,
	MENU,
	EDIT,
	QUIT,
}		GAME_STATE;

typedef struct s_SDL
{
	SDL_Event e;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
}			t_SDL;

typedef struct s_game
{
	char *name;
	t_SDL SDL;
	t_screen screen;
}			t_game;

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
		ply += player.z;
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

int	collisions(int *hm, t_point *player, int *height, int playerheight, int dx, int dy, int wallheight)
{
	if (collision_height(hm, player, height, playerheight) == 0)
		return (0);
	return (1);
}

int	init_game(t_game *game, t_SDL *SDL, int width, int height, char *name)
{
	game->screen.height = height;
	game->screen.width = width;
	game->screen.pixels = malloc(sizeof(uint32_t) * width * height);
	game->name = name;
	game->SDL = *SDL;
	game->SDL.window = NULL;
	game->SDL.renderer = NULL;
	game->SDL.texture = NULL;

	if(0 != SDL_Init(SDL_INIT_VIDEO))
		return(-1);
	game->SDL.window = SDL_CreateWindow(game->name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			game->screen.width, game->screen.height, SDL_WINDOW_SHOWN);
	if(NULL == game->SDL.window)
	return(-1);
	game->SDL.renderer = SDL_CreateRenderer(game->SDL.window, -1, SDL_RENDERER_SOFTWARE);
	if(NULL == game->SDL.renderer)
		return(-1);
	game->SDL.texture = SDL_CreateTexture(game->SDL.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
	if (NULL == game->SDL.texture)
		return(-1);

	return(0);
}

int	init_player(t_player *player, t_map *map)
{
	player->view_direction = 0;
	player->life = 100;
	player->pos.x = map->width/2;
	player->pos.z = map->height/2;
	player->pos.y = 100;
	player->fov = 90;
	player->horizon = 650;
	player->view_distance = 3000;
	player->speed = 10;
	return (0);
}

int	init_map(t_map *map, t_bitmap_texture *heightmap, t_bitmap_texture *colormap, char *name)
{
	if (heightmap->head.height != colormap->head.height)
		return (-1);
	if (heightmap->head.width != colormap->head.width)
		return (-1);
	map->height = heightmap->head.height;
	map->width = heightmap->head.width;
	map->heightmap = heightmap->pixels;
	map->colormap = colormap->pixels;
	map->name = name;
	return (0);
}

int	deal_event(t_game *game, t_player *player, int *quit, int *cursor, int *hm)
{
	if (game->SDL.e.type == SDL_QUIT) *quit = 1;
			if (game->SDL.e.type == SDL_KEYDOWN)
			{
				if (game->SDL.e.key.keysym.sym == SDLK_SPACE)
					player->pos.y += 5;
				if (game->SDL.e.key.keysym.sym == SDLK_x)
					player->pos.y -= 5;
				if (game->SDL.e.key.keysym.sym == SDLK_r)
					player->horizon += 5;
				if (game->SDL.e.key.keysym.sym == SDLK_f)
					player->horizon -= 5;
				if (game->SDL.e.key.keysym.sym == SDLK_w)
				{
					player->pos.z -= player->speed * (float)cos(player->view_direction);//10 == player player->speed;
					player->pos.x -= player->speed * (float)sin(player->view_direction);
				}
				if (game->SDL.e.key.keysym.sym == SDLK_s)
				{
					player->pos.z += player->speed * (float)cos(player->view_direction);//10 == player player->speed;
					player->pos.x += player->speed * (float)sin(player->view_direction);
				}
				if (game->SDL.e.key.keysym.sym == SDLK_a)
				{
					player->pos.z -= player->speed * (float)cos(player->view_direction + M_PI/2);//10 == player player->speed;
					player->pos.x -= player->speed * (float)sin(player->view_direction + M_PI/2);
				}
				if (game->SDL.e.key.keysym.sym == SDLK_d)
				{
					player->pos.z += player->speed * (float)cos(player->view_direction + M_PI/2);//10 == player player->speed;
					player->pos.x += player->speed * (float)sin(player->view_direction + M_PI/2);
				}
				if (game->SDL.e.key.keysym.sym == SDLK_ESCAPE)
					*quit = 1;
				if (game->SDL.e.key.keysym.sym == SDLK_LSHIFT)
					player->speed = 20;
				if (game->SDL.e.key.keysym.sym == SDLK_LCTRL)
				{
					*cursor = *cursor ? 0:1;
					cursor ? SDL_ShowCursor(SDL_ENABLE) : SDL_ShowCursor(SDL_DISABLE);
				}
				if (game->SDL.e.key.keysym.sym == SDLK_p)
					bomb(hm, abs(player->pos.x % (WIDTH * MAPEXPOSANT)), abs(player->pos.z % (HEIGHT * MAPEXPOSANT)), 50, player->pos.y);
			}
			if (game->SDL.e.type == SDL_KEYUP)
			{
				if (game->SDL.e.key.keysym.sym == SDLK_LSHIFT)
					player->speed = 10;
			}
			if (game->SDL.e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (game->SDL.e.button.button == SDL_BUTTON_LEFT)
					printf("height:%d\n", player->pos.y);
				if (game->SDL.e.button.button == SDL_BUTTON_RIGHT)
					printf("x:%d y: %d\n", player->pos.x, player->pos.z);
			}
			if (game->SDL.e.type == SDL_MOUSEMOTION)
			{
				if (game->SDL.e.button.x > WIDTH / 2)
					player->view_direction -= 0.01 * (game->SDL.e.button.x - WIDTH / 2);
				if (game->SDL.e.button.x < WIDTH / 2)
					player->view_direction += 0.01 * (WIDTH/2 - game->SDL.e.button.x );
				if (game->SDL.e.button.y > HEIGHT / 2)
					player->horizon -= 1 * (game->SDL.e.button.y - HEIGHT / 2);
				if (game->SDL.e.button.y < HEIGHT / 2)
					player->horizon += 1 * (HEIGHT/2 - game->SDL.e.button.y);
			}
	return (0);
}

int main(int argc, char **argv)
{
	t_game game;
	t_SDL SDL;
	int quit = 0;
	if (init_game(&game, &SDL, 1024, 1024, "DOOM") == -1)
		goto Quit;
	int statut = EXIT_FAILURE;

	t_bitmap_texture *bg = load_bmp("assets/sky/sky.bmp");
	t_map map;
	init_map(&map,	load_bmp("assets/maps/volcano/heightmap.bmp"),
					load_bmp("assets/maps/volcano/colormap.bmp"),
					"volcano");
	SDL_ShowCursor(SDL_DISABLE);
	SDL_RenderClear(game.SDL.renderer);

	SDL_SetRenderDrawBlendMode(game.SDL.renderer, SDL_BLENDMODE_NONE);
	SDL_SetTextureBlendMode(game.SDL.texture, SDL_BLENDMODE_NONE);

	memset(game.screen.pixels, 0x00000000, game.screen.height * game.screen.width * sizeof(uint32_t));

	t_player player;
	init_player(&player, &map);

	int *hm;
	hm = malloc(sizeof(int) * HEIGHT * WIDTH);
	memset(hm, 0, WIDTH * HEIGHT * sizeof(int));
	for(int i = 0; i < WIDTH * HEIGHT - 1; i++)
		hm[i] = get_blue(map.heightmap[i]);
	int cursor = 0;
	while(!quit)
	{
		if(!cursor)
			SDL_WarpMouseInWindow(game.SDL.window, WIDTH / 2, HEIGHT / 2);

		while(SDL_PollEvent(&game.SDL.e))
			deal_event(&game, &player, &quit, &cursor, hm);

		collision_height(hm, &player.pos, &player.pos.y, 10);

		render(game.screen.pixels, hm, map.colormap, player.pos, player.view_direction, player.pos.y, player.horizon, 2, player.view_distance, bg->pixels);

		SDL_UpdateTexture(game.SDL.texture, NULL, game.screen.pixels, game.screen.width * sizeof(uint32_t));
		SDL_RenderClear(game.SDL.renderer);
		SDL_RenderCopy(game.SDL.renderer, game.SDL.texture, NULL, NULL);
		SDL_RenderPresent(game.SDL.renderer);
	}

	statut = EXIT_SUCCESS;

Quit:
	if(NULL != game.SDL.renderer)
		SDL_DestroyRenderer(game.SDL.renderer);
	if(NULL != game.SDL.texture)
		SDL_DestroyTexture(game.SDL.texture);
	if(NULL != game.SDL.window)
		SDL_DestroyWindow(game.SDL.window);
	SDL_Quit();
	return statut;
}
