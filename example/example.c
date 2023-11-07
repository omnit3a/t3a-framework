#include <SDL2/SDL.h>
#include <t3a.h>

void test_input(){
  printf("Input\n");
}

void test_timer(){
  printf("Timer Finished\n");
}

int main(int argc, char ** argv){

  int quit = 0;
  screen_t screen;
  tea_init_screen(&screen);
  tea_init_controls();
  tea_init_timers();
  tea_register_control(SDLK_SPACE, &test_input);
  tea_start_timer(1000, &test_timer);
  
  while (!quit){
    
    if (tea_handle_input() < 0){
      quit = 1;
    }

    tea_handle_timers();
    
    for (int i = 0 ; i < 16 ; i++){  
      tea_set_pixel(i, i, 0);
    }
    tea_draw_screen(&screen);
  }
  
  tea_destroy_screen(&screen);
  
  return 0;
}
