#include <SDL2/SDL.h>
#include "../t3a.h"

float sprite_x_offset = 0;
float sprite_y_offset = 0;

void input_up(){
  sprite_y_offset--;
}

void input_down(){
  sprite_y_offset++;
}

void input_left(){
  sprite_x_offset--;
}

void input_right(){
  sprite_x_offset++;
}

void test_timer(){
  printf("Timer finished\n");
}

int main(int argc, char ** argv){

  int quit = 0;
  screen_t screen;
  sprite_t sprite;
    
  tea_init_screen(&screen);
  tea_init_controls();
  tea_init_timers();
  tea_init_sprite(&sprite, "example/bmp-example.bmp"); 

  tea_register_control(SDLK_w, &input_up);
  tea_register_control(SDLK_s, &input_down);
  tea_register_control(SDLK_a, &input_left);
  tea_register_control(SDLK_d, &input_right);
  
  tea_start_timer(1000, &test_timer);
  
  while (!quit){

    SDL_Delay(10);
    if (tea_handle_input() < 0){
      quit = 1;
    }
    tea_handle_timers();

    sprite.target.x = sprite_x_offset;
    sprite.target.y = sprite_y_offset;
    tea_draw_sprite(&screen, &sprite);

    tea_draw_screen(&screen);
  }
  
  tea_destroy_screen(&screen);
  
  return 0;
}
