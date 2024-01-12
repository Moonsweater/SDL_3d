#include <SDL2/SDL.h>
#include "Polygons.c"


void draw_light(SDL_Renderer *rend, int x, int y, int shine_length, int inner_radius) {

  //SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

  int R = shine_length + inner_radius;
  //Radius should eventually depend on brightness of light,
  //AND DISTANCE FROM VIEWPORT.
  
  for (int i=shine_length; i > 0; i--) {
    
    double r = i;
    double r_over_dist = r / shine_length;
    double brightness = r_over_dist - 2*sqrt(r_over_dist) - 1;
    int brightnessI = 255*brightness;
    
    SDL_SetRenderDrawColor(rend, brightnessI, brightnessI, brightnessI, 0);
    
    //Sadly, alpha channel shenanigans get in the way. What I REALLY want is to draw these things on with replacement onto an extra buffer, then add an alpha channel to that buffer, and slap it into the images. Alas. 
    
    //Since we're drawing each radius one-by-one, non-filled circles SHOULD work.
   
    thick_circle(rend, x, y, r + inner_radius);
    
  }
  
  SDL_SetRenderDrawColor(rend, 255, 255, 255, 0);
  
  for (int i = inner_radius; i > 0; i--) {
    thick_circle(rend, x, y, i);
  }
  
}

int main() {

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("error initializing SDL: %s\n", SDL_GetError());
    return 1;
  }
  
  SDL_Window *win = SDL_CreateWindow("0_0", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 600, 600,0);

  Uint32 render_flags = SDL_RENDERER_ACCELERATED;    
  SDL_Renderer* r = SDL_CreateRenderer(win, -1, render_flags);
  
  SDL_SetRenderDrawColor(r, 100, 0, 0, 0);
  
  SDL_RenderClear(r);
  
  SDL_SetRenderDrawColor(r, 255, 255, 255, 0);
  
  draw_light(r, 300, 300, 200, 20);
  
  //thick_circle(r, 200, 200, 100);
  
  SDL_RenderPresent(r);
  
  SDL_Delay(5000);

}
