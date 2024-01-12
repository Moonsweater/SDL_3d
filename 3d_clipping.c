#ifndef CLIPPING_3D
#define CLIPPING_3D

#include "3d_drawing_globals.c"
#include <stdbool.h>

point3 compute_plane_intersection(plane clipping_plane, point3 p0, point3 p1) {

  //Use parametric equation for line.
  //Find t such that line(t) - clipping_plane.base_point is orthogonal to clipping_plane.normal_vec.
  
  //line(t) = at + b, p = plane base point. a, b, p are points.
  //We seek (at + b - p)^T * v = 0.
    //Rearrange: [a b p] * [t 1 -1]^T * v = 0
    
    // [t 1 -1] * [a b p]^T * v = 0 
      
    //            [a1 a2 a3]   [v1]
    // [t 1 -1] * |b1 b2 b3| * |v2| = 0
    //            [p1 p2 p3]   [v3]
    
    //            [dot(a,v)]
    // [t 1 -1] * |dot(b,v)| = 0
    //            [dot(p,v)]
    
    // t * dot(a,v) + dot(b,v) - dot(p,v) = 0
    // t = dot(p-b, v) / dot(a,v);
    
    vec a, v, p_minus_b;
    
    set_vec(&a, p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
    //vec b;
    //set_vec(&b, p0.x, p0.y, p0.z);
    v = clipping_plane.normal_vector;
    
    double adot = dot(a, v);
    
    if (adot == 0 ) {
      printf("Panicking! polygon edge lies precisely on clipping plane.\n");
      printf("input any integer to continue anyway.\n");
      int junk;
      scanf("%d", &junk);
    }
    
   set_vec(
     &p_minus_b, 
     clipping_plane.base_point.x-p0.x, 
     clipping_plane.base_point.y-p0.y, 
     clipping_plane.base_point.z-p0.z
   ); 
    
    double t = dot(p_minus_b, v) / adot;
    
    double epsilon = 0.00001;
    
    if (!(0 - epsilon <= t && t <= 1 + epsilon)) {
      printf("Panicking! line-plane intersection doesn't actually lie on the line *segment*.\n");
      printf("input any integer to continue anyway.\n");
      int junk;
      scanf("%d", &junk);
    }
    
    point3 out;
    set_point3(&out, t*(p1.x - p0.x) + p0.x, t*(p1.y - p0.y) + p0.y, t*(p1.z - p0.z) + p0.z);
    
    return out;
    
}

bool same_side(plane p, point3 a, point3 b) {

  vec A, B;
  
  set_vec(&A, a.x - p.base_point.x, a.y - p.base_point.y, a.z - p.base_point.z);
  set_vec(&B, b.x - p.base_point.x, b.y - p.base_point.y, b.z - p.base_point.z);
  
  return dot(A, p.normal_vector) * dot(B, p.normal_vector) >= 0;
  
}

void handle_bad_good(point3 new_data[], int *new_length, bool *prev_is_good, int which_plane, point3 prev, point3 current) {

  if (*prev_is_good) {
    if (same_side(clipping_box[which_plane], prev, current)) {
      //good to good.
      new_data[*new_length] = current;
      *new_length = *new_length + 1;
      *prev_is_good = true;
    } else {
      //good to bad.
      new_data[*new_length] = compute_plane_intersection(clipping_box[which_plane], prev, current);
      *new_length = *new_length + 1;
      *prev_is_good = false;
    }
  } else {
    if (!same_side(clipping_box[which_plane], prev, current)) {
      //bad to good.
      new_data[*new_length] = compute_plane_intersection(clipping_box[which_plane], prev, current);
      *new_length = *new_length + 1;
      new_data[*new_length] = current;
      *new_length = *new_length + 1;
      *prev_is_good = true;     
    } else {
      //bad to bad.
      //do nothing.
    }
  }
}

void single_clip(point3 clipped_data[], int *clipping_length, int which_plane) {

  //Still looks like it's unsafe?
  //returns the new length.
  
  if (*clipping_length < 3) {return;}
 
  int new_length = 0;
  int old_length = *clipping_length;
  point3 current, prev;
  point3 guaranteed_good;
  point3 old_data[old_length];
   
  current = clipped_data[0]; 
  set_point3(&guaranteed_good, 0, 0, (clipping_box[0].base_point.z + clipping_box[5].base_point.z) * 0.5);
  for (int i=0; i < old_length; i++) {
    old_data[i] = clipped_data[i];
  }
  
  bool prev_is_good = same_side(clipping_box[which_plane], current, guaranteed_good);
  
  if (prev_is_good) {
    clipped_data[0] = current;
    new_length++;
  }
  
  for (int i=1; i < old_length; i++) {
    prev = current;
    current = old_data[i];
    handle_bad_good(clipped_data, &new_length, &prev_is_good, which_plane, prev, current);
  }
  
  //Connect first and last points in the instance that they're both good.
  
  prev = current; current = old_data[0];  
  if (!same_side(clipping_box[which_plane], prev, current)) {
    clipped_data[new_length] = compute_plane_intersection(clipping_box[which_plane], prev, current);
    new_length++;
  }
  
  *clipping_length = new_length;
  
  return;
  
}

#endif
