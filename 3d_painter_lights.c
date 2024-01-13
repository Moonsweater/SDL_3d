#ifndef PAINTER_LIGHTS_3D
#define PAINTER_LIGHTS_3D

#include "3d_drawing_globals.c"
#include "3d_lighting.c"
#include "3d_clipping.c"
#include "Polygons.c"

//Sorting:
//==================================

void set_color(SDL_Renderer* r, color c) {
  SDL_SetRenderDrawColor(r, c.r, c.g, c.b, 255);
  return;
}

int z_polygon_compare(const void *a, const void *b) {
  
  polygon_sorting_data *A, *B;
  A = (polygon_sorting_data*)a; B = (polygon_sorting_data*)b;
  
  if ((*A).z_avg < (*B).z_avg) {return -1;}
  else if ((*A).z_avg > (*B).z_avg) {return 1;}
  else {return 0;}

}

double z_polygon_avg(int obj, int p) {

  //p for polygon, not p for point.

  double sum = 0; 
  int numpoints = polygons[obj][p].points_per_polygon;
  for (int i = 0; i < numpoints; i++) {
    sum += points[obj][polygons[obj][p].which_points[i]].z;
  }    
  
  //printf("z_polygon_avg()\n");
  return sum / numpoints;   
}

void update_z_averages() {
  int o, p;
  for (int i=0; i < num_polygons; i++) {
    o = polygon_order[i].object;
    p = polygon_order[i].polygon;
    polygon_order[i].z_avg = -fabs(z_polygon_avg(o,p));
  }
  //printf("update_z_averages\n");
  return;
}

void z_sort_points() {
  update_z_averages();
  qsort(polygon_order, num_polygons, sizeof(polygon_sorting_data), z_polygon_compare);
  //printf("z_sort_points()\n");
  return;
}

//Drawing:
//==================================

void draw_polygon(SDL_Renderer *r, int o, int p) {
  
  //printf("Drawing polygon o = %d, p = %d.\n", o, p);

  double intensity = compute_lighting_intensity(o,p);
  //double intensity = 0.4;
  
  set_color(r, apply_lighting_to_colors(polycolors[o][p], intensity));
  
  int unclipped_length = polygons[o][p].points_per_polygon;
  point3 clipped_points[MAX_POINTS_PER_POLYGON];
  double x[MAX_POINTS_PER_POLYGON], y[MAX_POINTS_PER_POLYGON], z;
  
  for (int i=0; i < unclipped_length; i++) {
    clipped_points[i] = points[o][polygons[o][p].which_points[i]];
  }
  
  int clipped_length = unclipped_length;
  for (int i=0; i < 6; i++) {
    single_clip(clipped_points, &clipped_length, i);
    //updates clipped_length as it goes
  }
  
  for (int i=0; i < clipped_length; i++) {
  
    z = clipped_points[i].z;  
  
    if (fabs(z) > 0.001) {
      x[i] = viewport_scale_x * clipped_points[i].x / z;
      x[i] = x[i] + WIN_X/2; 
      y[i] = viewport_scale_y * clipped_points[i].y / z; 
      y[i] = -y[i] + WIN_Y/2; 
   } else {
      x[i] = WIN_X / 2;
      y[i] = WIN_Y / 2;
   } 
    
  } 
  
  fill_polygon(r, x, y, clipped_length);
  
  //printf("draw_polygon()\n");
  
  return;
  
}

void draw_objects(SDL_Renderer *r) {

  z_sort_points();
  
  SDL_SetRenderDrawColor(r, 0,0,0,255);
  SDL_RenderClear(r);

  int o, p;
  for (int i=0; i < num_polygons; i++) {
    
    o = polygon_order[i].object;
    p = polygon_order[i].polygon;
    
    draw_polygon(r, o, p);
    //printf("Polygon Drawing %d Complete. z average equals %lf \n", i, polygon_order[i].z_avg);
    
  }
 //printf("draw_object()\n");
 return;
}


#endif 
