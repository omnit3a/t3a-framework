#include <SDL2/SDL.h>
#include "../t3a.h"

int color_offset = 0;
int color_direction = 1;

void test_input(){
  printf("Space pressed\n");
}

void test_timer(){
  color_offset++;
  tea_start_timer(50, &test_timer);
}

int main(int argc, char ** argv){

  int quit = 0;
  screen_t screen;
  tea_init_screen(&screen);
  tea_init_controls();
  tea_init_timers();
  tea_register_control(SDLK_SPACE, &test_input);
  tea_start_timer(50, &test_timer);
  
  while (!quit){
    
    if (tea_handle_input() < 0){
      quit = 1;
    }
    tea_handle_timers();

    for (int x = 0 ; x < SCREEN_WIDTH ; x++){
      for (int y = 0 ; y < SCREEN_HEIGHT ; y++){  
	tea_set_pixel(&screen, (y + x + color_offset) % 16, x, y);
      }
    }
    tea_draw_screen(&screen);
  }
  
  tea_destroy_screen(&screen);
  
  return 0;
}
