#include <SDL2/SDL.h>
#include "../t3a.h"

int sprite_x_offset = 0;
int sprite_y_offset = 0;

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

int main(){

  int quit = 0;
  screen_t screen;
  sprite_t sprite;
  sprite_atlas_t sprite_atlas;
  font_t font;
  
  tea_init_screen(&screen);
  tea_init_ttf();
  tea_init_controls();
  tea_init_timers();
  tea_init_sprite(&sprite, "example/assets/bmp-example.bmp");
  tea_init_atlas(&sprite_atlas, "example/assets/atlas-example.bmp", 8, 8);
  tea_init_font(&font, "example/assets/font-example.ttf");
  
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

    tea_set_sprite_position(&sprite, sprite_x_offset, sprite_y_offset);
    tea_draw_sprite(&screen, &sprite);
    tea_draw_atlas(&screen, &sprite_atlas, 0);
    tea_draw_text(&screen, &font, "Hello, world", 0, 0);
    
    tea_draw_screen(&screen);
  }

  tea_destroy_ttf();
  tea_destroy_screen(&screen);
  
  return 0;
}
