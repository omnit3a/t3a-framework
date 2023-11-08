#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 96

#define CONTROL_MAX 64
#define TIMER_MAX 64
#define PATH_LENGTH 256
#define SFX_MAX 64

typedef void (*func_t)();

typedef struct screen_s {
  SDL_Window * window;
  SDL_Renderer * renderer;
  SDL_Surface * surface;
  SDL_Texture * texture;
} screen_t;

typedef struct control_s {
  SDL_Keycode keycode;
  void (*function)();
} control_t;

typedef struct control_list_s {
  int in_use[CONTROL_MAX];
  control_t info[CONTROL_MAX];
} control_list_t;

typedef struct tea_timer_s {
  Uint32 start;
  Uint32 length;
  void (*function)();
} tea_timer_t;

typedef struct timer_list_s {
  int in_use[TIMER_MAX];
  tea_timer_t info[TIMER_MAX];
} timer_list_t;

typedef struct origin_s {
  int relative_x;
  int relative_y;
} origin_t;

typedef struct sprite_s {
  origin_t origin;
  SDL_Rect target;
  SDL_Rect clip;
  char path[PATH_LENGTH];
} sprite_t;

typedef struct sprite_atlas_s {
  int current_tile;
  origin_t origin;
  SDL_Rect atlas;
  SDL_Rect tile;
  char path[PATH_LENGTH];
} sprite_atlas_t;

typedef struct font_s {
  int width;
  int height;
  TTF_Font * font;
  SDL_Color color;
  char path[PATH_LENGTH];
} font_t;

control_list_t controls;
timer_list_t timers;

static inline int tea_init_screen(screen_t * screen){
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
    fprintf(stderr, "Failed to initialize SDL2\n");
    return 0;
  }
  
  screen->window = SDL_CreateWindow("t3a",
				    SDL_WINDOWPOS_UNDEFINED,
				    SDL_WINDOWPOS_UNDEFINED,
				    SCREEN_WIDTH * 4,
				    SCREEN_HEIGHT * 4,
				    0);
  assert(screen->window);
  
  screen->renderer = SDL_CreateRenderer(screen->window, -1, 0);
  assert(screen->renderer);
  SDL_RenderSetLogicalSize(screen->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

  screen->surface = SDL_GetWindowSurface(screen->window);
  assert(screen->surface);
  
  screen->texture = SDL_CreateTexture(screen->renderer,
				      SDL_PIXELFORMAT_RGBA8888,
				      SDL_TEXTUREACCESS_STATIC,
				      SCREEN_WIDTH,
				      SCREEN_HEIGHT);
  assert(screen->texture);
  
  SDL_SetRenderDrawColor(screen->renderer, 0, 0, 0, 255);
  SDL_RenderClear(screen->renderer);
  printf("Successfully initialized game window\n");
  return 1;
}

static inline void tea_init_sprite(sprite_t * sprite, char * path){
  SDL_Surface * temp_surface = SDL_LoadBMP(path);
  if (!temp_surface){
    fprintf(stderr, "Could not load image %s\n", path);
    return;
  }
  strncpy(sprite->path, path, PATH_LENGTH);
  sprite->origin.relative_x = (temp_surface->w)/2;
  sprite->origin.relative_y = (temp_surface->h)/2;
  sprite->target = (SDL_Rect){0-sprite->origin.relative_x, 0-sprite->origin.relative_y, 0, 0};
  sprite->clip = (SDL_Rect){0, 0, 0, 0};
  sprite->target.w = temp_surface->w;
  sprite->target.h = temp_surface->h;
  SDL_FreeSurface(temp_surface);
}

static inline void tea_set_sprite_position(sprite_t * sprite, int x, int y){
  sprite->target.x = x - sprite->origin.relative_x;
  sprite->target.y = y - sprite->origin.relative_y;
}

static inline void tea_draw_sprite(screen_t * screen, sprite_t * sprite){
  SDL_Surface * bmp = SDL_LoadBMP(sprite->path);
  if (!bmp){
    return;
  }
  screen->texture = SDL_CreateTextureFromSurface(screen->renderer, bmp);

  sprite->target.w = bmp->w;
  sprite->target.h = bmp->h;
  
  SDL_RenderCopy(screen->renderer, screen->texture, NULL, &sprite->target);
  SDL_DestroyTexture(screen->texture);
  SDL_FreeSurface(bmp);
}

static inline void tea_init_atlas(sprite_atlas_t * sprite_atlas, char * path, int tile_width, int tile_height){
  SDL_Surface * temp_surface = SDL_LoadBMP(path);
  if (!temp_surface){
    fprintf(stderr, "Could not load image %s\n", path);
    return;
  }
  strncpy(sprite_atlas->path, path, PATH_LENGTH);

  sprite_atlas->origin.relative_x = tile_width/2;
  sprite_atlas->origin.relative_y = tile_height/2;
  
  sprite_atlas->atlas.x = 0-sprite_atlas->origin.relative_x;
  sprite_atlas->atlas.y = 0-sprite_atlas->origin.relative_y;;
  sprite_atlas->atlas.w = tile_width;
  sprite_atlas->atlas.h = tile_height;

  sprite_atlas->tile.x = 0;
  sprite_atlas->tile.y = 0;
  sprite_atlas->tile.w = tile_width;
  sprite_atlas->tile.h = tile_height;

  sprite_atlas->current_tile = 0;
  SDL_FreeSurface(temp_surface);
}

static inline void tea_set_atlas_position(sprite_atlas_t * sprite_atlas, int x, int y){
  sprite_atlas->atlas.x = x - sprite_atlas->origin.relative_x;
  sprite_atlas->atlas.y = y - sprite_atlas->origin.relative_y;
}

static inline void tea_draw_atlas(screen_t * screen, sprite_atlas_t * sprite_atlas, int tile){
  SDL_Surface * bmp = SDL_LoadBMP(sprite_atlas->path);
  if (!bmp){
    return;
  }
  screen->texture = SDL_CreateTextureFromSurface(screen->renderer, bmp);

  if (sprite_atlas->current_tile != tile){
    if (!tile){
      sprite_atlas->tile.x = 0;
      sprite_atlas->tile.y = 0;
    } else {
      sprite_atlas->tile.x = (tile % (bmp->w / sprite_atlas->tile.w)) * sprite_atlas->tile.w;
      sprite_atlas->tile.y = (tile / (bmp->h / sprite_atlas->tile.h)) * sprite_atlas->tile.h;
    }
    sprite_atlas->current_tile = tile;
  }

  SDL_RenderCopy(screen->renderer, screen->texture, &sprite_atlas->tile, &sprite_atlas->atlas);
  SDL_DestroyTexture(screen->texture);
  SDL_FreeSurface(bmp);  
}

static inline void tea_draw_screen(screen_t * screen){
  SDL_RenderPresent(screen->renderer);
  SDL_SetRenderDrawColor(screen->renderer, 0, 0, 0, 255);
  SDL_RenderClear(screen->renderer);
}

static inline void tea_destroy_screen(screen_t * screen){
  SDL_DestroyTexture(screen->texture);
  SDL_DestroyRenderer(screen->renderer);
  SDL_DestroyWindow(screen->window);
  SDL_Quit();
  printf("Goodbye!\n");
}

static inline void tea_init_ttf(){
  TTF_Init();
}

static inline void tea_destroy_ttf(){
  TTF_Quit();
}

static inline void tea_init_font(font_t * font, char * path){
  font->width = 0;
  font->height = 0;
  font->font = TTF_OpenFont(path, 8);
  if (!font){
    fprintf(stderr, "Could not load font %s\n", path);
    return;
  }
  font->color = (SDL_Color){255, 255, 255, 255};
  strncpy(font->path, path, PATH_LENGTH);
}

static inline void tea_draw_text(screen_t * screen, font_t * font, char * string, int x, int y){
  SDL_Surface * text = TTF_RenderText_Solid(font->font, string, font->color);
  if (!text){
    return;
  }
  screen->texture = SDL_CreateTextureFromSurface(screen->renderer, text);
  SDL_QueryTexture(screen->texture, NULL, NULL, &font->width, &font->height);
  SDL_Rect target;
  target.x = x * font->width;
  target.y = y * font->height;
  target.w = font->width;
  target.h = font->height;
  SDL_RenderCopy(screen->renderer, screen->texture, NULL, &target);

  SDL_DestroyTexture(screen->texture);
  SDL_FreeSurface(text);
}

static inline void tea_destroy_font(font_t * font){
  TTF_CloseFont(font->font);
}

static inline void tea_init_controls(){
  for (int index = 0 ; index < CONTROL_MAX ; index++){
    controls.in_use[index] = 0;
  }
}

static inline void tea_register_control(SDL_Keycode keycode, func_t function){
  for (int index = 0 ; index < CONTROL_MAX ; index++){
    if (controls.in_use[index]){
      continue;
    }
    controls.in_use[index] = 1;
    controls.info[index].keycode = keycode;
    controls.info[index].function = function;
    return;
  }
  printf("Could not register control: control scheme full\n");
}

static inline int tea_handle_input(){
  SDL_Event event;
  SDL_PollEvent(&event);
  if (event.type == SDL_QUIT){
    return -1;
  }

  const Uint8 * current_key_states = SDL_GetKeyboardState(NULL);
  for (int index = 0 ; index < CONTROL_MAX ; index++){
    if (!controls.in_use[index]){
      continue;
    }
    if (!current_key_states[SDL_GetScancodeFromKey(controls.info[index].keycode)]){
      continue;
    }
    controls.info[index].function();
  }  
  return 1;
}

static inline void tea_init_timers(){
  for (int index = 0 ; index < TIMER_MAX ; index++){
    timers.in_use[index] = 0;
  }
}

static inline int tea_start_timer(Uint32 length, func_t function){
  for (int index = 0 ; index < TIMER_MAX ; index++){
    if (timers.in_use[index]){
      continue;
    }
    timers.in_use[index] = 1;
    timers.info[index].start = SDL_GetTicks();
    timers.info[index].length = length;
    timers.info[index].function = function;
    return 1;
  }
  return 0;
}

static inline void tea_handle_timers(){
  for (int index = 0 ; index < TIMER_MAX ; index++ ){
    if (!timers.in_use[index]){
      continue;
    }

    if (SDL_GetTicks() - timers.info[index].start < timers.info[index].length){
      continue;
    }
    timers.in_use[index] = 0;
    timers.info[index].function();
  }
}
