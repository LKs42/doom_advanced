#include "SDL.h"
#include <stdio.h>
#include "libft.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitmap/bitmap.h"
#include "error/error.h"

//#define RATIO 16 / 9
#define HEIGHT 1080
#define WIDTH 1920
#define MAPEXPOSANT 1
//#define WIDTH (HEIGHT * RATIO)
#include <math.h>

typedef struct	s_point
{
	int x;
	int y;
	int z;
}		t_point;

typedef struct	s_screen
{
	int width;
	int height;
	uint32_t *pixels;
}		t_screen;

typedef struct	s_map
{
	char *name;
	int scale;
	int width;
	int height;
	int *heightmap;
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

typedef struct s_button
{
	t_list_head	node;
	t_point pos;
	int width;
	int height;
	int	id;
	int	clicked;
	uint32_t color;
}		t_button;

typedef struct s_game
{
	char *name;
	GAME_STATE STATE;
	t_SDL SDL;
	t_screen screen;
	int quit;
}			t_game;

typedef struct s_monster
{
	char *name;
	int life;
	t_point pos;
	int range_detection;
	float speed;
}			t_monster;

float lerp(float v0, float v1, float t)
{
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

uint32_t	darken(uint32_t color, int p)
{
	uint32_t r;
	uint32_t g;
	uint32_t b;

	r = (color >> 16) & 255;
	g = (color >> 8) & 255;
	b = color & 255;

	r /= p;
	g /= p;
	b /= p;

	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;

	color = r << 16 | g << 8 | b;
	return (color);
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

void	rect(uint32_t *pixels, int x, int y, int width, int height, uint32_t color)
{
	int i;
	int j;

	i = 0;
	while(i < height)
	{
		j = 0;
		while(j < width)
		{
			pixels[(y + i) * WIDTH + (x + j)] = color;
			j++;
		}
		i++;
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

void	draw_vertical_line_bot(uint32_t *pixels, int x, int ytop, int ybot, uint32_t color)
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

void	draw_vertical_line_top(uint32_t *pixels, int x, int ytop, int ybot, uint32_t color)
{
	int tmp;
	if (ytop > ybot)
		return ;
	if (ytop < 0)
		ytop = 0;
	if (ybot < 0)
		ybot = 0;
	if (ybot > HEIGHT)
		ybot = HEIGHT;
	if (ytop > HEIGHT)
		ytop = HEIGHT;
	ytop -= HEIGHT / 2;
	tmp = ytop;
	ytop = (HEIGHT/2) - tmp;
	ybot -= HEIGHT / 2;
	tmp = ybot;
	ybot = (HEIGHT/2) - ybot;
	if (ytop < ybot)
		return ;
	if (ytop < 0)
		ytop = 0;
	if (ybot < 0)
		ybot = 0;
	if (ybot > HEIGHT)
		ybot = HEIGHT;
	if (ytop > HEIGHT)
		ytop = HEIGHT;
	while (ytop > ybot)
		pixels[(--ytop) * WIDTH + x] = color;
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

void	draw_hud(uint32_t *pixels, uint32_t *texture)
{
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		if (texture[i] <= 0xF0F0F0) pixels[i] = texture[i];
}

void	draw_bg(uint32_t *pixels, uint32_t *texture)
{
	for (int i = 0; i < HEIGHT * WIDTH; i++)
		pixels[i] = texture[i];
}

void	draw_heightmap(uint32_t *pixels, int *texture, int x, int y)
{
	for (int i = 0; i < 1024; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			pixels[(i + y) * WIDTH + (j + x)] = 0x010101 * texture[i * 1024 + j];
		}
	}
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

int	compute_distance(int x1, int y1, int x2, int y2)
{
	int x_dist;
	int y_dist;

	x_dist = abs(x2 - x1);
	y_dist = abs(y2 - y1);

	return((int)sqrt((x_dist * x_dist) + (y_dist * y_dist)));
}

uint32_t	light(uint32_t color, float z, int distance, int value)
{
	uint32_t r;
	uint32_t g;
	uint32_t b;

	r = (color >> 16) & 255;
	g = (color >> 8) & 255;
	b = color & 255;

	r /= (z * value / distance);
	g /= (z * value / distance);
	b /= (z * value / distance);

	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;

	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;

	color = r << 16 | g << 8 | b;
	return (color);
}

void	fill_pixels(uint32_t *pixels, uint32_t color)
{
	for (int i = 0; i < HEIGHT * WIDTH; i++)
		pixels[i] = color;
}

void	render(t_screen *screen, t_map *map, t_player *camera, t_bitmap_texture *background, t_bitmap_texture *hud)
{
	uint32_t *bg = background->pixels;
	uint32_t *cockpit = hud->pixels;
	uint32_t *pixels = screen->pixels;
	uint32_t *colormap = map->colormap;
	t_point *player = &camera->pos;
	float *phi = &camera->view_direction;
	int *height = &camera->pos.y;
	int *horizon = &camera->horizon;
	int scale_height = map->scale;
	int distance = camera->view_distance;
	memset(pixels, 0x000000, sizeof(uint32_t) * screen->width * screen->height);
	//draw_bg(pixels, bg);
	//fill_pixels(pixels, 0x000000);
	int mapwidthperiod = map->width - 1;
	int mapheightperiod = map->height - 1;


	float sinang = sin(*phi);
	float cosang = cos(*phi);

	float deltaz = 1;
	uint32_t hiddeny[screen->width];
	for(int i = 0; i < screen->width; i++)
		hiddeny[i] = screen->height;
	uint32_t hiddeny2[screen->width];
	for(int i = 0; i < screen->width; i++)
		hiddeny2[i] = screen->height;

	for(float z=1; z < distance; z += deltaz)
	{
		float plx =  -cosang * z - sinang * z;
		float ply =   sinang * z - cosang * z;
		float prx =   cosang * z - sinang * z;
		float pry =  -sinang * z - cosang * z;
		float dx = (prx - plx) / screen->width;
		float dy = (pry - ply) / screen->width;
		plx += player->x;
		ply += player->z;
		float invz = 1 / z * 240 * scale_height;
		int mapoffset = 0;
		for(int i=0; i < screen->width; i++)
		{
			mapoffset = (((int)floorf(ply) & (int)mapwidthperiod) << 10) + (((int)floorf(plx)) & ((int)mapheightperiod));
			float heightonscreen = ((*height) - map->heightmap[mapoffset]) * invz + (*horizon + 512);
			float heightonscreen2 = ((400 -*height) - map->heightmap[mapoffset]) * invz - (*horizon - 512);
			draw_vertical_line_top(pixels, i, heightonscreen2, hiddeny2[i], light(colormap[mapoffset], z, distance, 10));
			draw_vertical_line_bot(pixels, i, heightonscreen, hiddeny[i], light(colormap[mapoffset], z, distance, 10));
			if (heightonscreen < hiddeny[i]) hiddeny[i] = heightonscreen;
			if (heightonscreen2 < hiddeny2[i]) hiddeny2[i] = heightonscreen2;
			if (hiddeny[i] > hiddeny2[i]) hiddeny[i] = hiddeny2[i];
			plx += dx;
			ply += dy;
		}
		deltaz += 0.005;
	}
	draw_hud(pixels, cockpit);
}

int	collision_height(int *hm, t_point *player, int *height, int playerheight)
{
	int x;
	int y;

	x = abs(player->x % 1024);
	y = abs(player->z % 1024);
	if (*height < hm[y * 1024 + x] + playerheight)
		*height = hm[y * 1024 + x] + playerheight;
	if (*height >= 300)
		*height = 300;
	return (1);
}

int	collisions(t_map *map, t_point *player, int *height, int playerheight, int dx, int dy, int wallheight)
{
	if (collision_height(map->heightmap, player, height, playerheight) == 0)
		return (0);
	return (1);
}

int	init_game(t_game *game, t_SDL *SDL, int width, int height, char *name)
{
	game->screen.height = height;
	game->screen.width = width;
	game->name = name;
	game->SDL = *SDL;
	game->SDL.window = NULL;
	game->SDL.renderer = NULL;
	game->SDL.texture = NULL;
	game->STATE = MENU;
	game->quit = 0;
	if(!(game->screen.pixels = malloc(sizeof(uint32_t) * width * height)))
		return (-1);
	if(0 != SDL_Init(SDL_INIT_VIDEO))
		return(-1);
	game->SDL.window = SDL_CreateWindow(game->name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			game->screen.width, game->screen.height, SDL_WINDOW_SHOWN);
	if(NULL == game->SDL.window)
		return(-1);
	game->SDL.renderer = SDL_CreateRenderer(game->SDL.window, -1, SDL_RENDERER_SOFTWARE);
	if(NULL == game->SDL.renderer)
		return(-1);
	game->SDL.texture = SDL_CreateTexture(game->SDL.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
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
	player->view_distance = 1000;
	player->speed = 10;
	return (0);
}

int	init_map(t_map *map, t_bitmap_texture *heightmap, t_bitmap_texture *colormap, char *name)
{
	int *hm;
	if (heightmap->head.height != colormap->head.height)
		return (-1);
	if (heightmap->head.width != colormap->head.width)
		return (-1);
	map->scale = 2;
	if (heightmap == NULL || colormap == NULL)
		return (-1);
	map->height = heightmap->head.height;
	map->width = heightmap->head.width;
	map->colormap = colormap->pixels;
	map->name = name;
	if(!(hm = malloc(sizeof(int) * map->height * map->width)))
		return (-1);
	map->heightmap = hm;
	memset(hm, 0, map->width * map->height * sizeof(int));
	for(int i = 0; i < map->width * map->height - 1; i++)
		hm[i] = get_blue(heightmap->pixels[i]);
	free(heightmap);
	return (0);
}

void	move_forward(t_player *player, t_point *direction)
{
	direction->y -= player->speed * (float)cos(player->view_direction);
	direction->x -= player->speed * (float)sin(player->view_direction);
}

void	move_backward(t_player *player, t_point *direction)
{
	direction->y += player->speed * (float)cos(player->view_direction);
	direction->x += player->speed * (float)sin(player->view_direction);
}

void	move_left(t_player *player, t_point *direction)
{
	direction->y -= player->speed * (float)cos(player->view_direction + M_PI/2);
	direction->x -= player->speed * (float)sin(player->view_direction + M_PI/2);
}

void	move_right(t_player *player, t_point *direction)
{
	direction->y += player->speed * (float)cos(player->view_direction + M_PI/2);
	direction->x += player->speed * (float)sin(player->view_direction + M_PI/2);
}

void	process_continuous_key(t_game *game, t_player *player, t_point *direction)
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	direction->x = 0;
	direction->y = 0;
	player->speed = 10;
	if (keystate[SDL_SCANCODE_SPACE]) player->pos.y += 5;
	if (keystate[SDL_SCANCODE_LCTRL]) player->pos.y -= 5;
	if (keystate[SDL_SCANCODE_LSHIFT]) player->speed = 20;
	if (keystate[SDL_SCANCODE_W]) move_forward(player, direction);
	if (keystate[SDL_SCANCODE_S]) move_backward(player, direction);
	if (keystate[SDL_SCANCODE_A]) move_left(player, direction);
	if (keystate[SDL_SCANCODE_D]) move_right(player, direction);
}

int	game_event(t_game *game, t_player *player, t_point *direction)
{
	if (game->SDL.e.type == SDL_KEYUP)
	{
		if (game->SDL.e.key.keysym.sym == SDLK_LSHIFT)
			player->speed = 10;
	}
	if (game->SDL.e.type == SDL_MOUSEBUTTONDOWN)
	{
		if (game->SDL.e.button.button == SDL_BUTTON_LEFT)
			printf("height:%d\n", player->pos.y);
//			printf("horizon:%d\n", player->horizon);
		if (game->SDL.e.button.button == SDL_BUTTON_RIGHT)
			printf("x:%d y: %d\n", player->pos.x, player->pos.z);
	}
	if (game->SDL.e.type == SDL_MOUSEMOTION)
	{
		if (game->SDL.e.button.x > game->screen.width / 2)
			player->view_direction -= 0.01 * (game->SDL.e.button.x - game->screen.width / 2);
		if (game->SDL.e.button.x < game->screen.width / 2)
			player->view_direction += 0.01 * (game->screen.width/2 - game->SDL.e.button.x );
		if (game->SDL.e.button.y > game->screen.height / 2)
			player->horizon -= 1 * (game->SDL.e.button.y - game->screen.height / 2);
		if (game->SDL.e.button.y < game->screen.height / 2)
			player->horizon += 1 * (game->screen.height/2 - game->SDL.e.button.y);
	}

	return (0);
}

int	deal_event(t_game *game, t_player *player, t_point *direction)
{
	if (game->SDL.e.type == SDL_QUIT) game->quit = 1;
	if (game->SDL.e.type == SDL_KEYUP)
	{
		if (game->SDL.e.key.keysym.sym == SDLK_ESCAPE) game->quit = 1;
		if (game->SDL.e.key.keysym.sym == SDLK_F1) game->STATE = GAME;
		if (game->SDL.e.key.keysym.sym == SDLK_F2) game->STATE = MENU;
	}
	if (game->STATE == GAME)
		game_event(game, player, direction);
	return (0);
}

void	process_input(t_game *game, t_player *player, t_point *direction)
{
	if(game->STATE == GAME) process_continuous_key(game, player, direction);
	while(SDL_PollEvent(&game->SDL.e))
		deal_event(game, player, direction);
	player->pos.x += direction->x;
	player->pos.z += direction->y;
}

void	quit_doom(t_game *game)
{
	if(NULL != game->SDL.renderer)
		SDL_DestroyRenderer(game->SDL.renderer);
	if(NULL != game->SDL.texture)
		SDL_DestroyTexture(game->SDL.texture);
	if(NULL != game->SDL.window)
		SDL_DestroyWindow(game->SDL.window);
	SDL_Quit();
	exit(0);
}

t_button *button(int x, int y, int width, int height)
{
	t_button *result;

	if(!(result = malloc(sizeof(t_button))))
		return (NULL);
	result->pos.x = x;
	result->pos.y = y;
	result->width = width;
	result->height = height;
	result->clicked = 0;
	return (result);
}

// t_button	**init_menu()
// {
// 	t_button **list;

// 	int size = 250;

// 	if(!(list = malloc(sizeof(t_button) * 3)))
// 		return (0);
// 	list[0] = button(WIDTH/2 - size/2, HEIGHT/2 , size, 50);
// 	list[1] = button(WIDTH/2 - size/2, HEIGHT/2 + 60, size, 50);
// 	list[2] = button(WIDTH/2 - size/2, HEIGHT/2 + 120, size, 50);
// 	list[3] = NULL;
// 	return (list);
// }


void	add_button_menu(t_point pos, int width, int height, t_list_head *button_list)
{
	t_button	*nbutton;

	nbutton = button(pos.x, pos.y, width, height);
	init_list_head(&nbutton->node);
	if (button_list->next != button_list)
		nbutton->id = ((t_button*)button_list->prev)->id + 1;
	else
		nbutton->id = 1;
	list_add_entry(&nbutton->node, button_list);
}

void	init_menu(t_list_head *button_list, int nb)
{
	t_list_head *pos;
	int			i;
	// int 		size;
	t_button	*nbutton;
	int			height;

	i = 0;
	// size = 500;
	height = HEIGHT / 2 - 500 / nb;
	while (i < nb)
	{
		nbutton = button(WIDTH/2 - 500/2, height, 500, 150);
		nbutton->id = i + 1;
		init_list_head(&nbutton->node);
		height += 190;
		list_add_entry(&nbutton->node, button_list);
		i++;
	}
	// if(!(list = malloc(sizeof(t_button) * 3)))
	// 	return (0);
	// list[0] = button(WIDTH/2 - size/2, HEIGHT/2 , size, 50);
	// list[1] = button(WIDTH/2 - size/2, HEIGHT/2 + 60, size, 50);
	// list[2] = button(WIDTH/2 - size/2, HEIGHT/2 + 120, size, 50);
	// list[3] = NULL;
	// return ;
}

int	button_hover(t_button *button, int x, int y)
{
	if (x >= button->pos.x && x <= button->pos.x + button->width)
	{
		if (y >= button->pos.y && y <= button->pos.y + button->height)
		{
			return (1);
		}
	}
	return (0);
}

int	button_click(t_button *button, SDL_MouseButtonEvent mouse)
{
	if (button_hover(button, mouse.x, mouse.y) == 1 && mouse.button == SDL_BUTTON_LEFT && mouse.state == SDL_PRESSED)
	{
		// printf("button id = %d\n", button->id);
		return (1);
	}
	return (0);
}

void	render_button(t_screen *screen, t_button *button, SDL_MouseButtonEvent mouse)
{
	int i;
	int j;

	i = 0;
	while (i < button->height)
	{
		j = 0;
		while(j < button->width)
		{
			if (button_click(button, mouse) == 1)
				screen->pixels[(button->pos.y + i) * WIDTH + (button->pos.x + j)] = 0xFF00FF00;
			else
				screen->pixels[(button->pos.y + i) * WIDTH + (button->pos.x + j)] = button_hover(button, mouse.x, mouse.y) ? 0xFFFF00FF : 0xFF00FFFF;
			j++;
		}
		i++;
	}
}

void	switch_menu_edit(t_game *game)
{
	if (game->STATE == MENU)
		game->STATE = EDIT;
	else if (game->STATE == EDIT)
		game->STATE = MENU;
}

void	switch_menu_game(t_game *game)
{
	if (game->STATE == GAME)
		game->STATE = MENU;
	else if (game->STATE == MENU)
		game->STATE = GAME;
}

void	manage_button(t_button* button, SDL_MouseButtonEvent mouse, t_game *game)
{
	if (button_click(button, mouse) == 1)
	{
		if (button->id == 1 && button->clicked == 0)
		{
			switch_menu_game(game);
			button->clicked = 1;
		}
		if (button->id == 2 && button->clicked == 0)
		{
			switch_menu_edit(game);
			button->clicked = 1;
		}
		if (button->id == 3 && button->clicked == 0)
		{
			quit_doom(game);
		}
	}
	else
		button->clicked = 0;
}

void	act_button(t_list_head *button_list, SDL_MouseButtonEvent mouse, t_game *game)
{
	t_list_head	*pos;

	pos = button_list->next;
	while (pos != button_list)
	{
		manage_button(((t_button*)pos), mouse, game);
		pos = pos->next;
	}
}
// void	render_menu(t_screen *screen, t_button **list, int x, int y)
// {
// 	int i;
// 	i = 0;

// 	while(i < 3)
// 	{
// 		render_button(screen, list[i], x, y);
// 		i++;
// 	}
// }

void	render_menu(t_screen *screen, t_list_head *button_list, SDL_MouseButtonEvent mouse)
{
	t_list_head	*pos;

	pos = button_list->next;
	while(pos != button_list)
	{
		render_button(screen, ((t_button*)pos), mouse);
		pos = pos->next;
	}
}

void	draw_menu(t_screen *screen, t_bitmap_texture *background)
{
	uint32_t	*menu_pixels;
	uint32_t	*bg;

	menu_pixels = screen->pixels;
	bg = background->pixels;
	memset(menu_pixels, 0xFFFFFFFF, sizeof(uint32_t) * screen->width * screen->height);
	draw_bg(menu_pixels, bg);
}

void	draw_edit(t_screen *screen, t_map *map, t_bitmap_texture *background)
{
	uint32_t	*edit_pixels;
	uint32_t	*bg;

	bg = background->pixels;
	edit_pixels = screen->pixels;
	memset(edit_pixels, 0xFFFFFFFF, sizeof(uint32_t) * screen->width * screen->height);
	draw_bg(edit_pixels, bg);
	draw_heightmap(edit_pixels, map->heightmap, (WIDTH - map->width ) / 2, (HEIGHT - map->height) / 2);
}

int main(int argc, char **argv)
{
	t_game game;
	t_SDL SDL;
	if (init_game(&game, &SDL, WIDTH, HEIGHT, "DOOM") == -1)
		goto Quit;
	int statut = EXIT_FAILURE;

	t_bitmap_texture *bg = load_bmp("assets/sky/sky1080.bmp");
	t_bitmap_texture *cockpit = load_bmp("assets/cockpit1080.bmp");
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
	int cursor = 0;
	t_point direction;
	direction.x = 0;
	direction.y = 0;
	// t_button **list = init_menu();
	t_list_head	button_list;
	init_list_head(&button_list);
	init_menu(&button_list, 3);
	t_list_head	edit_button_list;
	t_point	pos;
	pos.x = 0;
	pos.y = HEIGHT / 2 - (2 * (160 / 2));
	init_list_head(&edit_button_list);
	add_button_menu(pos, 200, 100, &edit_button_list);
	pos.y += 120;
	add_button_menu(pos, 200, 100, &edit_button_list);
	pos.y += 120;
	add_button_menu(pos, 200, 100, &edit_button_list);
	// add_button_menu(pos, 400, 80, &button_list);
	// goto Quit;

	double currenttime;
	double rendercount = 0;
	double lasttime = 0;
	double avgFPS;
	while(!game.quit)
	{
		currenttime = SDL_GetTicks();
		avgFPS = (float)rendercount / ((float)currenttime / 1000.0f);
		if (currenttime > lasttime + 1000)
		{
			printf("%f \n", avgFPS);
			if(avgFPS > 20000000)
				avgFPS = 0;
			lasttime = currenttime;
		}
		if (game.STATE == MENU)
			cursor = 1;
		if (game.STATE == GAME)
			cursor = 0;
		cursor ? SDL_ShowCursor(SDL_ENABLE) : SDL_ShowCursor(SDL_DISABLE);
		if(!cursor)
			SDL_WarpMouseInWindow(game.SDL.window, game.screen.width / 2, game.screen.height / 2);
		process_input(&game, &player, &direction);
		collision_height(map.heightmap, &player.pos, &player.pos.y, 1);
		if (game.STATE == GAME)
		{
			render(&game.screen, &map, &player, bg, cockpit);
			rendercount++;
		}
		if (game.STATE == MENU)
			//render_menu(&game.screen, list, game.SDL.e.button.x, game.SDL.e.button.y);
		{
			draw_menu(&game.screen, bg);
			act_button(&button_list, game.SDL.e.button, &game);
			render_menu(&game.screen, &button_list, game.SDL.e.button);
		}
		if (game.STATE == EDIT)
		{
			draw_edit(&game.screen, &map, bg);
			render_menu(&game.screen, &edit_button_list, game.SDL.e.button);
		}
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
