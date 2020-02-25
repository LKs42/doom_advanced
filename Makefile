# **************** VARIABLES ****************
UNAME		=	$(shell uname)
NAME		=	doom-nukem

# SOURCES
SRCS_LIST	=	main.c														\
				bitmap/load_bmp.c											\
				error/throw_error.c											\
				error/throw_null.c											\
				error/throw_void.c


SRCS_FOLDER	=	./srcs/
SRCS		=	$(addprefix $(SRCS_FOLDER), $(SRCS_LIST))

# OBJECTS
OBJS_LIST	=	$(SRCS_LIST:.c=.o)
OBJS_FOLDER	=	./objs/
OBJS		=	$(addprefix $(OBJS_FOLDER), $(OBJS_LIST))

# HEADERS
INCLUDES_FOLDER	=	./includes/
INCLUDES	:=	-I includes

# COMPILATION
CC			=	gcc
CFLAGS		:=	#-Wall -Werror -Wextra 
LDFLAGS		:=

# LIBRARIES
#  libft
LIBFT_FOLDER=	./libft
LIBFT		=	$(LIBFT_FOLDER)/libft.a
INCLUDES	:=	$(INCLUDES) -I $(LIBFT_FOLDER)/includes
LDFLAGS		:=	$(LDFLAGS) -L $(LIBFT_FOLDER) -lft -lm

#  SDL
SDL_FOLDER		=	./SDL
SDL				=	$(SDL_FOLDER)/build
SDL_CONFIGURE	=	$(SDL_FOLDER)/aclocal.m4
INCLUDES		:=	$(INCLUDES) -I $(SDL_FOLDER)/include
LDFLAGS			:=	$(LDFLAGS) -L $(SDL_FOLDER) -lSDL2

#  SDL Mixer
SDL_MIXER_FOLDER	=	./SDL_mixer
SDL_MIXER			=	$(SDL_MIXER_FOLDER)/build
SDL_MIXER_CONFIGURE	=	$(SDL_MIXER_FOLDER)/autom4te.cache
INCLUDES			:=	$(INCLUDES) -I $(SDL_MIXER_FOLDER)
LDFLAGS				:=	$(LDFLAGS) -Wl,-rpath=$(SDL_MIXER_FOLDER)/build/.libs -L $(SDL_MIXER_FOLDER)/build/.libs -lSDL2_mixer

#  SDL TTF
SDL_TTF_FOLDER		=	./SDL_ttf
SDL_TTF				=	$(SDL_TTF_FOLDER)/.libs
SDL_TTF_CONFIGURE	=	$(SDL_TTF_FOLDER)/autom4te.cache
INCLUDES			:=	$(INCLUDES) -I $(SDL_TTF_FOLDER)
LDFLAGS				:=	$(LDFLAGS) -Wl,-rpath=$(SDL_TTF_FOLDER) -L $(SDL_TTF_FOLDER) -lSDL2_ttf

# **************** RULES ****************

all: $(SDL) $(SDL_MIXER) $(SDL_TTF) $(LIBFT) $(NAME)

$(NAME): $(OBJS)
	@printf "Linking...\n"
	@$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)

run: $(ALL)
	@./$(NAME)

$(OBJS_FOLDER)%.o: $(SRCS_FOLDER)%.c
	@mkdir -p $(dir $@)
	@$(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

$(LIBFT):
	@make -C libft

$(SDL_CONFIGURE):
	@printf "Configuring SDL...\n"
	@cd $(SDL_FOLDER) && \
	./autogen.sh && \
	./configure

$(SDL): $(SDL_CONFIGURE)
	@printf "Compiling SDL...\n"
	@cd $(SDL_FOLDER) && \
	make

$(SDL_MIXER_CONFIGURE):
	@printf "Configuring SDL Mixer...\n"
	@cd $(SDL_MIXER_FOLDER) && \
	./autogen.sh && \
	./configure

$(SDL_MIXER): $(SDL_MIXER_CONFIGURE)
	@printf "Compiling SDL Mixer...\n"
	@cd $(SDL_MIXER_FOLDER) && \
	make

$(SDL_TTF_CONFIGURE):
	@printf "Configuring SDL TTF...\n"
	@cd $(SDL_TTF_FOLDER) && \
	./autogen.sh && \
	./configure

$(SDL_TTF): $(SDL_TTF_CONFIGURE)
	@printf "Compiling SDL TTF...\n"
	@cd $(SDL_TTF_FOLDER) && \
	make

clean: libft-clean sdl-clean
	@rm -rf $(OBJS_FOLDER)

mostlyclean:
	@rm -rf $(OBJS_FOLDER)

libft-clean:
	@make -C $(LIBFT_FOLDER) clean

sdl-clean:
	# @make -C $(SDL_FOLDER) clean

sdl-mixer-clean:
	@make -C $(SDL_MIXER_FOLDER) clean

sdl-ttf-clean:
	@make -C $(SDL_TTF_FOLDER) clean

clean-binary:
	@rm -rf $(NAME)

fcl: mostlyclean clean-binary
	@rm -rf $(NAME)

fclean: clean libft-fclean sdl-fclean
	@rm -rf $(NAME)

libft-fclean:
	@make -C $(LIBFT_FOLDER) fclean

sdl-fclean: sdl-clean
	@rm -rf $(SDL_FOLDER)/build

sdl-mixer-fclean: sdl-mixer-clean
	@rm -rf $(SDL_MIXER_FOLDER)/build

sdl-ttf-fclean: sdl-ttf-clean
	@rm -rf $(SDL_TTF_FOLDER)/.libs

r: mostlyclean all

re: fclean all

relink: clean-binary $(NAME)

.PHONY: clean mostlyclean libft-clean sdl-clean sdl-mixer-clean sdl-ttf-clean clean-binary fcl fclean libft-fclean sdl-fclean sdl-mixer-fclean sdl-ttf-fclean r re relink
