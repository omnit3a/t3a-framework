#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 64

#define HIGH_RES_COLORS 8
#define LOW_RES_COLORS 16

typedef void (*func_t)(void * args);

Uint16 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

typedef enum graphics_mode_e {
  HIGH_RES = 1,
  LOW_RES
} graphics_mode_t;

graphics_mode_t graphics_mode;

Uint16 high_res_colors[8] = {
  0x000f,
  0xf00f,
  0x0f0f,
  0x00ff,
  0xff0f,
  0xf0ff,
  0x0fff,
  0xffff,
};

Uint16 low_res_colors[16] = {
  0x000f,
  0xf00f,
  0x0f0f,
  0x00ff,
  0xff0f,
  0xf0ff,
  0x0fff,
  0xffff,
  0x555f,
  0xf55f,
  0x5f5f,
  0x55ff,
  0xff5f,
  0xf5ff,
  0x5fff,
  0xffff,
};

typedef struct screen_s {
  SDL_Window * window;
  SDL_Renderer * renderer;
  SDL_Texture * texture;
} screen_t;

static inline int init_screen(screen_t * screen){
  if (SDL_Init(SDL_INIT_VIDEO) < 0){
    fprintf(stderr, "Failed to initialize SDL2\n");
    return 0;
  }

  screen->window = SDL_CreateWindow("64Engine",
				    SDL_WINDOWPOS_UNDEFINED,
				    SDL_WINDOWPOS_UNDEFINED,
				    SCREEN_WIDTH * 8,
				    SCREEN_HEIGHT * 8,
				    0);
  if (!screen->window){
    fprintf(stderr, "Failed to create window\n");
    return 0;
  }
  
  screen->renderer = SDL_CreateRenderer(screen->window, -1, 0);
  if (!screen->renderer){
    fprintf(stderr, "Failed to create renderer\n");
    return 0;
  }
  
  screen->texture = SDL_CreateTexture(screen->renderer,
				      SDL_PIXELFORMAT_RGBA4444,
				      SDL_TEXTUREACCESS_STATIC,
				      SCREEN_WIDTH,
				      SCREEN_HEIGHT);
  if (!screen->texture){
    fprintf(stderr, "Failed to create texture\n");
    return 0;
  }

  SDL_UpdateTexture(screen->texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint16));
  printf("Successfully initialized game window\n");
  return 1;
}

static inline void set_pixel(Uint16 color_id, int x, int y){
  int x_off = x;
  int run = 1;
  int index = (y * SCREEN_WIDTH) + x_off;
  Uint16 color = color_id;

  if (graphics_mode == LOW_RES && x_off % 2 == 1){
    x_off++;
    index = (y * SCREEN_WIDTH) + x_off;
  }
  
  if (graphics_mode == LOW_RES){
    color = low_res_colors[color_id % 16];
    run = 2;
  } else {
    color = high_res_colors[color_id % 8];
    run = 1;
  }
  
  for (int width = 0 ; width < run ; width++){
    pixels[index + width] = color;
  }
}

static inline void draw_screen(screen_t * screen){
  SDL_UpdateTexture(screen->texture,
		    NULL,
		    pixels,
		    SCREEN_WIDTH * sizeof(Uint16));
  SDL_RenderCopy(screen->renderer, screen->texture, NULL, NULL);
  SDL_RenderPresent(screen->renderer);
}

static inline void destroy_screen(screen_t * screen){
  SDL_DestroyTexture(screen->texture);
  SDL_DestroyRenderer(screen->renderer);
  SDL_DestroyWindow(screen->window);
  SDL_Quit();
  printf("Goodbye!\n");
}
