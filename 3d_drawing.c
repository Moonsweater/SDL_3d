#include "3d_main_loop.c"

//IT IS DONE!

//Fun bonus: implement phong or gourand shading.

//Less-fun bonus: draw the light source as a little bright circle. (Problem: our sorting method for painter's is built for polygons exclusively. And, the corona of a partially covered light source probably won't be displayed correctly.

//==========================================================

int main(int argc, char** argv) {
  
  if (argc <= 1) {printf("Expected at least one argument. \n"); exit(0);}

  FILE *f;
  num_objects = 0; num_polygons = 0;
  while (num_objects+1 < argc) {
    f = fopen(argv[num_objects+1], "r");
    read_xyz(f, num_objects);
    num_objects++;
  }
  
  //printf("%d total polygons. \n", num_polygons);
 
  int current_object = 0;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("error initializing SDL: %s\n", SDL_GetError());
    return 1;
  }
  
  SDL_Window *win = SDL_CreateWindow("0_0", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, WIN_X, WIN_Y,0);

  Uint32 render_flags = SDL_RENDERER_ACCELERATED;    
  SDL_Renderer* r = SDL_CreateRenderer(win, -1, render_flags);
  
  initialize_globals();
  
  M3d_begin_transformations();
  
  int tick_counter;
  
  while (1) {
  
    SDL_Delay(16);
    
    //print_object(current_object);
    
    handle_events(win, &current_object);
    
    //printf("Event queue emptied.\n");
    
    handle_transformations_and_drawing(r, &tick_counter, current_object);
    
    //scanf("%d", &junk);
    
  }     
   
  SDL_DestroyWindow(win);

  SDL_Quit();   

}
