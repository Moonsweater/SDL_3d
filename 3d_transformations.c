#ifndef TRANSFORMATIONS_3D
#define TRANSFORMATIONS_3D

#include "3d_drawing_globals.c"

void M3d_begin_transformations() {
  num_transformations = 0;
  //printf("M3d_begin_transformations()\n");
  return;
}

void M3d_apply_transformations(int obj) {
  
  if (num_transformations == 0) {return;}

  double a[4][4];
  M3d_make_identity(a);
  
  for (int i=0; i < num_transformations; i++) {
    M3d_mat_mult(a, transformation[i], a);
  }
  
  //M3d_print_mat(a);
  
  M3d_mat_mult_points(points[obj], a, points[obj], points_per_object[obj]);
  /*
  for(int i=0; i < points_per_object[obj]; i++) {
    printf("This one is in M3d. \n x: %5.2lf y:%5.2lf z:%5.2lf \n", points[obj][i].x, points[obj][i].y, points[obj][i].z);
  }
  */
  //printf("M3d_apply_transformations()\n");
  
  return;
  
}
#endif
