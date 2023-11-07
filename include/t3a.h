#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 64
#define CONTROL_MAX 64
#define TIMER_MAX 64

typedef void (*func_t)();

Uint16 * pixels;
Uint16 palette[16] = {
  0x000f,
  0x445f,
  0x207f,
  0x32ff,
  0x603f,
  0xf03f,
  0x708f,
  0xf3df,
  0x072f,
  0x0f3f,
  0x278f,
  0x2eff,
  0x960f,
  0xfe3f,
  0x779f,
  0xffff,
};

typedef struct screen_s {
  SDL_Window * window;
  SDL_Renderer * renderer;
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

control_list_t controls;
timer_list_t timers;

static inline int tea_init_screen(screen_t * screen){
  if (SDL_Init(SDL_INIT_VIDEO) < 0){
    fprintf(stderr, "Failed to initialize SDL2\n");
    return 0;
  }

  screen->window = SDL_CreateWindow("t3a",
				    SDL_WINDOWPOS_UNDEFINED,
				    SDL_WINDOWPOS_UNDEFINED,
				    SCREEN_WIDTH * 8,
				    SCREEN_HEIGHT * 8,
				    0);
  assert(screen->window);
  
  screen->renderer = SDL_CreateRenderer(screen->window, -1, 0);
  assert(screen->renderer);
  SDL_RenderSetLogicalSize(screen->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
  
  screen->texture = SDL_CreateTexture(screen->renderer,
				      SDL_PIXELFORMAT_RGBA4444,
				      SDL_TEXTUREACCESS_STATIC,
				      SCREEN_WIDTH,
				      SCREEN_HEIGHT);
  assert(screen->texture);
  
  pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint16));
  memset(pixels, 0x00, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint16));
  SDL_UpdateTexture(screen->texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint16));
  printf("Successfully initialized game window\n");
  return 1;
}

static inline void tea_set_pixel(Uint16 color_id, int x, int y){
  assert(x >= 0 && x < SCREEN_WIDTH);
  assert(y >= 0 && y < SCREEN_HEIGHT);
  assert(color_id >= 0 && color_id < 16);
  
  int index = (y * SCREEN_WIDTH) + x;
  pixels[index] = palette[color_id % 16];
}

static inline Uint16 tea_get_pixel(int x, int y){
  assert(x >= 0 && x < SCREEN_WIDTH);
  assert(y >= 0 && y < SCREEN_HEIGHT);

  int index = (y * SCREEN_WIDTH) + x;
  return pixels[index];
}

static inline void tea_draw_screen(screen_t * screen){
  SDL_RenderClear(screen->renderer);
  SDL_UpdateTexture(screen->texture,
		    NULL,
		    pixels,
		    SCREEN_WIDTH * sizeof(Uint16));
  SDL_RenderCopy(screen->renderer, screen->texture, NULL, NULL);
  SDL_RenderPresent(screen->renderer);
}

static inline void tea_destroy_screen(screen_t * screen){
  free(pixels);
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

  if (event.type != SDL_KEYDOWN){
    return 0;
  }
  
  for (int index = 0 ; index < CONTROL_MAX ; index++){
    if (!controls.in_use[index]){
      continue;
    }
    if (event.key.keysym.sym != controls.info[index].keycode){
      continue;
    }
    controls.info[index].function();
    return 1;
  }  
  return 0;
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
    timers.info[index].function();
    timers.in_use[index] = 0;
  }
}
