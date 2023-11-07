#include <SDL2/SDL.h>
#include <64engine.h>

int main(int argc, char ** argv){

  screen_t screen;
  init_screen(&screen);
  graphics_mode = LOW_RES;
  
  SDL_Event event;
  int quit = 0;

  while (!quit){

    for (int i = 0 ; i < 32 ; i++){  
      set_pixel(i, i, 0);
    }
    SDL_WaitEvent(&event);
    draw_screen(&screen);
    if (event.type == SDL_QUIT){
      quit = 1;
    }
  }

  destroy_screen(&screen);
  
  return 0;
}
