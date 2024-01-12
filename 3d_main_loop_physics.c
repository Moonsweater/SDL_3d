#ifndef MAIN_LOOP_P_3D
#define MAIN_LOOP_P_3D

#include "3d_drawing_globals.c"
#include "3d_transformations.c"
#include "3d_lighting.c"
#include "3d_painter.c"
#include "3d_drawing_globals.c"

//velocity information.
struct {
  double U, D, L, R, x, y, z, plus, minus;
} vel[MAX_OBJECTS];

//Maybe make it vel[MAX_OBJECTS][10], and define ROTX, VUP, to be integers.

void init_velocities() {

  //WHAT DO YOU MEAN it's not a pointer or a vector? grumble grumble grumble.
  
  int num_keys = 9;

  for (int o=0; o < num_objects; o++) {
    double *V = &(vel[o].U);
    for (int i=0; i < num_keys; i++) {
      V[i] = 0;
    }
  }
  return;
}

bool handle_single_keypress(int *key) {

  //Return true if the key switches from not-pressed to pressed.
  
  int temp  = *key;
  *key = 1;
  shutting_off_graphics = false;
  return !temp;
  
}

void handle_physics(int current_object) {

  //needs to handle physics for ALL objects, but only keystrokes for the current object.

  int *K = &(keys.U);
  int num_keys = 9;
  double friction = 0.95;
  double epsilon = 0.01;
  //Stupid hack to turn my struct into an array. This is probably all kinds of evil.
  
  for (int o=0; o < current_object; o++) {
    double *V = &(vel[o].U);
    for (int i=0; i < num_keys; i++) {
      V[i] *= friction;
      shutting_off_graphics = shutting_off_graphics && (fabs(V[i]) < epsilon);
    }
  }
  
  double *V = &(vel[current_object].U);
  for (int i=0; i < num_keys; i++) {
    V[i] = friction * V[i] + 0.125 * K[i];    
    printf("%lf ", V[i]);
    shutting_off_graphics = shutting_off_graphics && (fabs(V[i]) < epsilon);
  }
  printf("\n");
  
  printf("v.x, however, equals %lf \n", vel[current_object].x);
  
  for (int o = (current_object+1); o < num_objects; o++) {
    double *V = &(vel[o].U);
    for (int i=0; i < num_keys; i++) {
      V[i] *= friction;  
      shutting_off_graphics = shutting_off_graphics && (fabs(V[i]) < epsilon);
    }
  }
   
  return;
  
}

//KEYBOARD IS GLOBAL. WE DO NOT HAVE TO WORRY ABOUT PASSING POINTERS TO IT.

void handle_events(SDL_Window *window, int *current_object) {

  //NOTE: To make this actually work with multiple objects, we have to buffer current_object like we do with updating_graphics.
  
  //Also: this is responsive in some cases, but not when lag is occurring: if we're lagging, you need to hold down a key for it to register. Why?

  SDL_Event e;
    while(SDL_PollEvent(&e)) {        
       if (e.type == SDL_QUIT) {
         SDL_DestroyWindow(window);
         SDL_Quit();   
         exit(0);
       }
      if (e.type == SDL_KEYUP) {
        //printf("Key up.\n");
        switch (e.key.keysym.sym) {
          case SDLK_DOWN: keys.D = 0; break;
          case SDLK_UP: keys.U = 0; break;
          case SDLK_LEFT: keys.L = 0; break;
          case SDLK_RIGHT: keys.R = 0; break;
          case SDLK_x: keys.x = 0; break;
          case SDLK_y: keys.y = 0; break;
          case SDLK_z: keys.z = 0; break;
          case SDLK_EQUALS: keys.plus = 0; break;
          case SDLK_MINUS: keys.minus = 0; break;  
        }
        if (0 == 
             keys.D + keys.U + keys.L + keys.R
             + keys.x + keys.y + keys.z
             + keys.plus + keys.minus
           ) {shutting_off_graphics = true;}
      }
      if (e.type == SDL_KEYDOWN) {
        //printf("Key down.\n");
        switch (e.key.keysym.sym) {
          case SDLK_DOWN:  
            updating_graphics = true;    
            keys.U = 0;  
            if (handle_single_keypress(&(keys.D))) {return; }
            //Returning early ensures that, if a key is pressed and released during a period of lag, the keypress is still registered by the program.
            break;
          case SDLK_UP: 
            updating_graphics = true;   
            keys.D = 0;   
            if (handle_single_keypress(&(keys.U))) {return;} break;
          case SDLK_LEFT: 
            keys.R = 0;
            updating_graphics = true;      
            if (handle_single_keypress(&(keys.L))) {return;} break;
          case SDLK_RIGHT: 
            keys.L = 0;
            updating_graphics = true;      
            if (handle_single_keypress(&(keys.R))) {return;} break;
          case SDLK_x:
            updating_graphics = true;      
            if (handle_single_keypress(&(keys.x))) {return;} break;
          case SDLK_y:
            updating_graphics = true;      
            if (handle_single_keypress(&(keys.y))) {return;} break;
          case SDLK_z:
            updating_graphics = true;      
            if (handle_single_keypress(&(keys.z))) {return;} break;
          case SDLK_EQUALS:
            keys.minus = 0;
            updating_graphics = true;      
            if (handle_single_keypress(&(keys.plus))) {return;} break;
          case SDLK_MINUS:
            keys.plus = 0;
            updating_graphics = true;      
            if (handle_single_keypress(&(keys.minus))) {return;} break;
          default:
            if (49 <= e.key.keysym.sym && e.key.keysym.sym <= 48 + num_objects) {
              (*current_object) = e.key.keysym.sym - 49;
              updating_graphics = true;
              //Off by one to make it indexed at zero internally, at one externally.
            } 
          break;
        
        }
      }
    }
    
    
    return;
    
}

void handle_transformations_and_drawing(SDL_Renderer *r, int *start_ticks, int current_object) {

  if (updating_graphics) {
  
      //printf("We're in translating mode! \n");
      double dx = vel[current_object].R - vel[current_object].L;
      double dy = vel[current_object].U - vel[current_object].D;
      double dz = vel[current_object].plus - vel[current_object].minus;
      double rx = 0.1 * vel[current_object].x;
      double ry = 0.1 * vel[current_object].y;
      double rz = 0.1 * vel[current_object].z;
      
     
      
      //printf("dx = %lf, dy = %lf, dz =  %lf. \n", dx, dy, dz);
      
      if (SDL_GetTicks() - *start_ticks < time_delay) {
        printf("No draw attempted. Ticks elapsed: %d \n", SDL_GetTicks() - *start_ticks);
        //If not enough time has passed, just compute the translations, but keep them in memory, without computing, drawing, or rendering the changes.
          translate_shape(dx, dy, dz);
          center[current_object].x += dx;
          center[current_object].y += dy;
          center[current_object].z += dz;
          
          rotate_about_center(rx, 'x', center[current_object]);
          rotate_about_center(ry, 'y', center[current_object]);
          rotate_about_center(rz, 'z', center[current_object]);
      } else {
        //Otherwise, do everything normally.
        printf("Draw attempted. Ticks elapsed: %d\n", SDL_GetTicks() - *start_ticks);
        //printf("Net left = %d \n", 3*(keys.right - keys.left));
        *start_ticks = SDL_GetTicks();
        translate_shape(dx, dy, dz);
        center[current_object].x += dx;
        center[current_object].y += dy;
        center[current_object].z += dz;
        rotate_about_center(rx, 'x', center[current_object]);
        rotate_about_center(ry, 'y', center[current_object]);
        rotate_about_center(rz, 'z', center[current_object]);
        M3d_apply_transformations(current_object);  
        //printf("about to draw object.\n");
        draw_objects(r);
        SDL_RenderPresent(r);
        M3d_begin_transformations();
      //It's in this order for a reason: we want begin_transformations to include ALL subsequent matrix multiplications, and for it to not reset in the middle of the loop each time.  
      //Same logic for something_changed.   
       updating_graphics = !shutting_off_graphics;
       //If we have the flag to shutoff our graphics on this display, do so.
     }
   }
}

#endif
