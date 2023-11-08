#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
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

typedef struct sprite_s {
  SDL_Rect target;
  SDL_Rect clip;
  char path[PATH_LENGTH];
} sprite_t;

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
  strncpy(sprite->path, path, PATH_LENGTH);
  sprite->target = (SDL_Rect){0, 0, 0, 0};
  sprite->clip = (SDL_Rect){0, 0, 0, 0};
  SDL_Surface * temp_surface = SDL_LoadBMP(sprite->path);
  if (!temp_surface){
    fprintf(stderr, "Could not load image %s\n", sprite->path);
    return;
  }
  sprite->target.w = temp_surface->w;
  sprite->target.h = temp_surface->h;
  SDL_FreeSurface(temp_surface);
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
