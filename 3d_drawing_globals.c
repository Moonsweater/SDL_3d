#ifndef GLOBALS_3D
#define GLOBALS_3D

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "M3d_extended.c"

#define MAX_OBJECTS 5
#define MAX_LIGHTS 3
#define MAX_POLYGONS 2000
#define MAX_POINTS_PER_POLYGON 20
#define MAX_POINTS 2000
#define WIN_X 600
#define WIN_Y 600

//Viewport Data:

const double half_angle = M_PI/12;
   //Aka 15 degrees. Very reasonable half-angle.
double viewport_scale_x, viewport_scale_y; 
  //scale factor to turn positions on the viewport to pixels on the screen.
  
//Clipping Data:

typedef struct{
  vec normal_vector;
  point3 base_point;
} plane;

plane clipping_box[6];

void init_clipping_data() {

  //Hither:
  set_point3(&(clipping_box[0].base_point), 0, 0, 10);
  set_vec(&(clipping_box[0].normal_vector), 0, 0, 1);
  
  //Yon:
  set_point3(&(clipping_box[5].base_point), 0, 0, 100);
  set_vec(&(clipping_box[5].normal_vector), 0, 0, -1);
  
  //All the others:
  
  for (int i=1; i <= 4; i++) {
    set_point3(&(clipping_box[4].base_point), 0, 0, 0);
  }
  
  vec temp1, temp2;
  
  double width = tan(half_angle);
  
  set_vec(&temp1, 0, 1, 0);
  set_vec(&temp2, width, 0, 1);
  clipping_box[1].normal_vector = cross(temp1, temp2);
  
  set_vec(&temp1, 0, 1, 0);
  set_vec(&temp2, -width, 0, 1);
  clipping_box[1].normal_vector = cross(temp1, temp2);
  
  set_vec(&temp1, 1, 0, 0);
  set_vec(&temp2, 0, width, 1);
  clipping_box[1].normal_vector = cross(temp1, temp2);
  
  set_vec(&temp1, 1, 0, 0);
  set_vec(&temp2, 0, -width, 1);
  clipping_box[1].normal_vector = cross(temp1, temp2);
  
  return;

}

//Color & Lighting:

typedef struct {
  Uint8 r;
  Uint8 g;
  Uint8 b;
} color;

void print_color(color c) {
  printf("%d, %d, %d \n", c.r, c.g, c.b);
}

color polycolors[MAX_OBJECTS][MAX_POLYGONS];

typedef struct {

  point3 position[MAX_LIGHTS];
  
  double ambient;
  double diffuse;
  int spec_power;
  int num_lights;
  
  portion
  
} lighting_data;

lighting_data lighting

void initialize_lighting_data(double amb, double diff, int spec) {

   //In an ideal world, this would be done on a per-material basis.

   lighting.ambient = amb;
   lighting.diffuse = diff;
   lighting.spec_power = spec;
   lighting.num_lights = 0;
   
   printf("initialize_lighting_data.\n");
   
}

void add_light(double x, double y, double z) {

  int L = lighting.num_lights;

  lighting.position[L].x = x;
  lighting.position[L].y = y;
  lighting.position[L].z = z;
  lighting.num_lights++;
  
  return;
}

//Point data:

typedef struct {
  int points_per_polygon;
  int which_points[MAX_POINTS_PER_POLYGON];   
} polygon_data;

point3 points[MAX_OBJECTS][MAX_POINTS];
point3 center[MAX_OBJECTS];

int polygons_per_object[MAX_OBJECTS];
int num_polygons;
int points_per_object[MAX_OBJECTS];
int num_objects;

typedef struct {
  bool is_light;
  int object;
  int polygon;
  double z;
} sorting_data;

polygon_data polygons[MAX_OBJECTS][MAX_POLYGONS];
polygon_sorting_data sorting_order[MAX_OBJECTS*MAX_POLYGONS + MAX_LIGHTS];

int points_per_polygon[MAX_OBJECTS][MAX_POLYGONS];

void sorting_order_init() {
  int i=0;
  for (int o = 0; o < num_objects; o++) {
    for (int p=0; p < polygons_per_object[o]; p++) {
      sorting_order[i].is_light = false;
      sorting_order[i].object = o;
      sorting_order[i].polygon = p;
      i++;
    }
  }
  for (int l = 0; l < lighting.num_lights; l++) {
    sorting_order[l].is_light = true;
    i++;
  }
  
  printf("sorting_order_init()\n");
  
  return;
}

//Keyboard / Main Loop:

typedef struct {
  int U, D, L, R, x, y, z, plus, minus;
} keyboard;

keyboard keys;

bool updating_graphics;
bool shutting_off_graphics;

int time_delay;

void initialize_keyboard() {

  //Change this if keyboard every has more than 9 entries.

  keys.U = 0;
  keys.D = 0;
  keys.L = 0;
  keys.R = 0;
  keys.x = 0;
  keys.y = 0;
  keys.z = 0;
  keys.plus = 0;
  keys.minus = 0;
  
  updating_graphics = false;
  shutting_off_graphics = false;
  
  printf("initialize_keyboard.\n");
  
  return;
  
}

//Utility Functions:

void read_xyz(FILE *f, int obj) {

  point3 sum;
  sum.x = sum.y = sum.z = 0;
  
  if (f == NULL) {
    printf("Cannot read file. \n");
    exit(0);
  }
  
  fscanf(f, "%d", &points_per_object[obj]);
  printf("%d points in object %d. \n", points_per_object[obj], obj);
  for (int i=0; i < points_per_object[obj]; i++) {
    fscanf(f, "%lf %lf %lf", &points[obj][i].x, &points[obj][i].y, &points[obj][i].z);
    sum.x += points[obj][i].x;
    sum.y += points[obj][i].y;
    sum.z += points[obj][i].z;
  }
  
  center[obj].x = sum.x / points_per_object[obj];
  center[obj].y = sum.y / points_per_object[obj];
  center[obj].z = sum.z / points_per_object[obj];
  
  fscanf(f, "%d", &polygons_per_object[obj]);
  num_polygons += polygons_per_object[obj];
  for (int p=0; p < polygons_per_object[obj]; p++) {
    fscanf(f, "%d", &polygons[obj][p].points_per_polygon);
    for (int i=0; i < polygons[obj][p].points_per_polygon; i++) {
      fscanf(f, "%d", &polygons[obj][p].which_points[i]);
    }
  }
  
  //printf("read_xyz()\n");
  
  return;
  
}

void print_object(int o) {

  printf("%dth Object: \n", o);
  
  
  printf("%d points:\n", points_per_object[o]);
  for (int i=0; i < points_per_object[o]; i++) {
    printf("%lf %lf %lf \n", points[o][i].x, points[o][i].y, points[o][i].z);
  }
  
  /*
  
  printf("Connectivity: \n");
  printf("%d \n", polygons_per_object[o]);
  for (int poly = 0; poly < polygons_per_object[o]; poly++) {
    int length = polygons[o][poly].points_per_polygon;
    printf("%d ", length);
    for (int i=0; i < length; i++) {
      printf("%d ", polygons[o][poly].which_points[i]);
    }
    printf("\n");
  }
  
  */
  
  /*
  
  printf("Colors: \n");
  for (int poly = 0; poly < polygons_per_object[o]; poly++) {
    print_color(polycolors[o][poly]);
  }
  
  */  
  
  return;

}

void initialize_polygon_colors() {
  for (int o = 0; o < num_objects; o++) {
    for (int p=0; p < polygons_per_object[o]; p++) {
      polycolors[o][p].r = 0;
      polycolors[o][p].g = 200
      ; // + floor(drand48() * 50);
      polycolors[o][p].b = 0;
    }
  }
  printf("initialize_polygon_colors()\n");
  return;
}

void initialize_globals() {

  //Should be called AFTER writing in your files and stuff.

  initialize_keyboard();  
  
  point3 light_pos; light_pos.x = 100; light_pos.y = 200; light_pos.z = 0;
  initialize_lighting_data(0.4, 0.4, 50);
  add_light(100, 200, 0);
  //Start by just doing one light. We can add more later.
    
  initialize_polygon_colors();
  sorting_order_init();
  time_delay = 33;
  viewport_scale_x = WIN_X * 0.5 / tan(half_angle);
  viewport_scale_y = WIN_Y * 0.5 / tan(half_angle);
  init_clipping_data();
  
  return;
  
}

#endif

